/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

// Note: Boost has a deprecation message telling us to use BOOST_BIND_GLOBAL_PLACEHOLDERS, but we don't use the global placeholders, so this is not a problem for us.
// This actually seems to be caused by a faulty include made by boost itself.
#include <boost/asio/connect.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#ifdef __linux__
#include <boost/asio/local/stream_protocol.hpp>
#endif
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <future>
#include <string>
#include <thread>
#include <queue>
#include <optional>

namespace beast     = boost::beast;         // from <boost/beast.hpp>
namespace http      = beast::http;          // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;          // from <boost/asio.hpp>
using tcp           = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
#ifdef __linux__
namespace local = boost::asio::local;
#endif

constexpr int RESTART_WAIT_TIME = 10;

#ifdef __linux__
#define NULLNEXUS_GETWS(code) \
    if (isunix)               \
    {                         \
        unixws->code;         \
    }                         \
    else                      \
    {                         \
        tcpws->code;          \
    }
#define NULLNEXUS_VALIDWS (isunix ? (unixws && unixws->is_open()) : (tcpws && tcpws->is_open()))
#else
#define NULLNEXUS_GETWS(code) \
    {                         \
        tcpws->code;          \
    }
#define NULLNEXUS_VALIDWS (tcpws && tcpws->is_open())
#endif

class WebSocketClient
{
    // Settings
    std::string host, port, endpoint;
    bool isunix;
    std::vector<std::pair<std::string, std::string>> custom_connect_headers;
    // Message callback
    std::function<void(std::string)> callback;

    // ASIO
    net::io_context ioc;
    std::optional<net::executor_work_guard<decltype(ioc.get_executor())>> work;
    std::optional<websocket::stream<tcp::socket>> tcpws;
#ifdef __linux__
    std::optional<websocket::stream<local::stream_protocol::socket>> unixws;
#endif
    beast::flat_buffer buf;

    // Delayed start (after failed connect)
    net::deadline_timer start_delay_timer = net::deadline_timer(ioc);
    // Message list with delivery when connected
    net::deadline_timer message_queue_timer = net::deadline_timer(ioc);
    std::queue<std::string> messages;

    // Worker thread
    std::optional<std::thread> worker;

    bool is_running = false;

    void log(std::string msg)
    {
        // std::cout << msg << std::endl;
    }

    void handle_handler_error(const boost::system::error_code &ec)
    {
        if (ec == net::error::basic_errors::operation_aborted)
            return;
        log(ec.message() + " " + std::to_string(ec.value()));
        scheduleDelayedStart();
    }

    // Function to run the internal ASIO loop
    void runIO()
    {
        ioc.run();
        log("IOC exited");
    }

    // Function gets called whenever a message or error is sent
    void handler_onread(const boost::system::error_code &ec, std::size_t)
    {
        if (ec)
        {
            // Let someone else handle this error
            handle_handler_error(ec);
            return;
        }
        // Send message to callback
        callback(beast::buffers_to_string(buf.data()));
        buf.clear();
        // we stop reading after this call. We need to restart the handler.
        startAsyncRead();
    }

    void handler_ontcpconnect(const boost::system::error_code &ec, std::promise<void> *ret)
    {
        if (ec)
        {
            log("Connection to server failed!");
            // Something is waiting for the first connection attempt to finish
            if (ret)
                ret->set_value();
            if (ec == net::error::basic_errors::operation_aborted)
                return;
            scheduleDelayedStart();
            return;
        }
        doWebsocketSetup(ret);
    }

    // Start async reading from ASIO websocket
    void startAsyncRead()
    {
        NULLNEXUS_GETWS(async_read(buf, beast::bind_front_handler(&WebSocketClient::handler_onread, this)))
    }

    void doWebsocketSetup(std::promise<void> *ret)
    {
        try
        {
            // Set a decorator to change the User-Agent of the handshake
            NULLNEXUS_GETWS(set_option(websocket::stream_base::decorator([&](websocket::request_type &req) {
                for (auto &entry : custom_connect_headers)
                {
                    req.set(entry.first, entry.second);
                }
                req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
            })));
            // Perform the websocket handshake
            NULLNEXUS_GETWS(handshake(host, endpoint))

            log("CO: Connected to the server.");
            // Something is waiting for the first connection attempt to finish
            if (ret)
                ret->set_value();

            startAsyncRead();
            // Send cached messages
            trySendMessageQueue();
        }
        catch (...)
        {
            // Some error. Trying again later.
            log("CO: Websocket setup failed!");
            // Something is waiting for the first connection attempt to finish
            if (ret)
                ret->set_value();
            scheduleDelayedStart();
        }
    }

    // Called by internalStart to run the actual connection code
    void doConnectionAttempt(std::promise<void> *ret = nullptr)
    {
        try
        {
            tcp::resolver resolver{ ioc };

#ifdef __linux__
            if (isunix)
            {
                local::stream_protocol::endpoint ep(host);

                // Create a new websocket, old one can't be used anymore after a .close() call
                unixws.emplace(ioc);

                // Connect to the websocket
                unixws->next_layer().connect(ep);
                doWebsocketSetup(ret);
            }
            else
            {
#endif
                // Look up the domain name
                auto const results = resolver.resolve(host, port);

                // Create a new websocket, old one can't be used anymore after a .close() call
                tcpws.emplace(ioc);

                net::async_connect(tcpws->next_layer(), results.begin(), results.end(), std::bind(&WebSocketClient::handler_ontcpconnect, this, std::placeholders::_1, ret));
#ifdef __linux__
            }
#endif
        }
        catch (...)
        {
            // Some error. Trying again later.
            log("CO: Connection to server failed!");
            // Something is waiting for the first connection attempt to finish
            if (ret)
                ret->set_value();
            scheduleDelayedStart();
        }
    }

    /* Functions for handling the sending of messages */
    void handle_timerMessageQueue(const boost::system::error_code &ec)
    {
        if (ec)
            return;
        trySendMessageQueue();
    }
    void trySendMessageQueue()
    {
        if (!NULLNEXUS_VALIDWS)
            return;

        while (messages.size())
        {
            try
            {
                if (!NULLNEXUS_VALIDWS)
                    throw std::exception();
                NULLNEXUS_GETWS(write(net::buffer(messages.front())));
                messages.pop();
            }
            catch (...)
            {
                message_queue_timer.cancel();
                message_queue_timer.expires_from_now(boost::posix_time::seconds(1));
                message_queue_timer.async_wait(std::bind(&WebSocketClient::handle_timerMessageQueue, this, std::placeholders::_1));
                return;
            }
        }
    }
    void onImmediateMessageSend(std::string msg, std::promise<bool> &ret)
    {
        try
        {
            if (!NULLNEXUS_VALIDWS)
            {
                ret.set_value(false);
                return;
            }
            NULLNEXUS_GETWS(write(net::buffer(msg)));
            ret.set_value(true);
        }
        catch (...)
        {
            ret.set_value(false);
            return;
        }
    }
    void onAsyncMessageSend(std::string msg)
    {
        // Push into a queue
        messages.push(msg);
        // Try to send said queue
        trySendMessageQueue();
    }
    /* ~Functions for handling the sending of messages~ */

    // React to the timer being activated
    void handler_startDelayTimer(const boost::system::error_code &ec)
    {
        if (ec)
            return;
        doConnectionAttempt();
    }

    // Use the io_context+worker to sheudule a restart/start
    void scheduleDelayedStart()
    {
        start_delay_timer.cancel();
        start_delay_timer.expires_from_now(boost::posix_time::seconds(RESTART_WAIT_TIME));
        start_delay_timer.async_wait(std::bind(&WebSocketClient::handler_startDelayTimer, this, std::placeholders::_1));
    }
    void internalStart(std::promise<void> *ret)
    {
        if (is_running)
        {
            if (ret)
                ret->set_value();
            return;
        }
        is_running = true;
        doConnectionAttempt(ret);
    }
    void internalStop(std::promise<void> &ret)
    {
        if (!is_running)
        {
            ret.set_value();
            return;
        }
        is_running = false;
        start_delay_timer.cancel();
        // Stop message queue from running while stopped
        message_queue_timer.cancel();
        if (tcpws)
            tcpws->next_layer().cancel();
        if (NULLNEXUS_VALIDWS)
        {
            NULLNEXUS_GETWS(close(websocket::close_code::normal));
        }
        ret.set_value();
    }

    void internalSetCustomHeaders(std::vector<std::pair<std::string, std::string>> headers, std::promise<void> &ret)
    {
        custom_connect_headers = headers;
        ret.set_value();
    }

public:
    void start(bool async = false)
    {
        if (async)
            // Let boost deal with anything related to thread safety
            net::post(ioc, std::bind(&WebSocketClient::internalStart, this, nullptr));
        else
        {
            std::promise<void> ret;
            auto future = ret.get_future();
            // Let boost deal with anything related to thread safety
            net::post(ioc, std::bind(&WebSocketClient::internalStart, this, &ret));
            future.wait();
        }
    }
    void stop()
    {
        std::promise<void> ret;
        auto future = ret.get_future();
        // Let boost deal with anything related to thread safety
        net::post(ioc, std::bind(&WebSocketClient::internalStop, this, std::ref(ret)));
        future.wait();
    }

    bool sendMessage(std::string msg, bool sendIfOffline = false)
    {
        if (sendIfOffline)
        {
            // Let the worker thread handle this safely
            net::post(ioc, std::bind(&WebSocketClient::onAsyncMessageSend, this, msg));
            return true;
        }
        else
        {
            std::promise<bool> ret;
            auto future = ret.get_future();
            // Let the worker thread handle this safely
            net::post(ioc, std::bind(&WebSocketClient::onImmediateMessageSend, this, msg, std::ref(ret)));
            future.wait();
            return future.get();
        }
    }

    void setCustomHeaders(std::vector<std::pair<std::string, std::string>> headers)
    {
        std::promise<void> ret;
        auto future = ret.get_future();
        net::post(ioc, std::bind(&WebSocketClient::internalSetCustomHeaders, this, headers, std::ref(ret)));
        future.wait();
    }

    WebSocketClient(std::string host, std::string port, std::string endpoint, std::function<void(std::string)> callback) : host(host), port(port), endpoint(endpoint), callback(callback)
    {
        isunix = false;
        work.emplace(ioc.get_executor());
        worker.emplace(std::bind(&WebSocketClient::runIO, this));
    }
#ifdef __linux__
    WebSocketClient(std::string unixsocket_addr, std::string endpoint, std::function<void(std::string)> callback) : host(unixsocket_addr), endpoint(endpoint), callback(callback)
    {
        isunix = true;
        work.emplace(ioc.get_executor());
        worker.emplace(std::bind(&WebSocketClient::runIO, this));
    }
#endif

    ~WebSocketClient()
    {
        stop();
        work.reset();
        worker->join();
    }
};

#undef NULLNEXUS_GETWS
#undef NULLNEXUS_VALIDWS
