/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

// Note: Boost has a deprecation message telling us to use BOOST_BIND_GLOBAL_PLACEHOLDERS, but we don't use the global placeholders, so this is not a problem for us.
// This actually seems to be caused by a faulty include made by boost itself.
#include <boost/asio/connect.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <future>
#include <string>
#include <thread>
#include <queue>
#include <optional>
#include <utility>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

constexpr int RESTART_WAIT_TIME = 10;

#define NULLNEXUS_GETWS(code) \
    {                         \
        TCPWS->code;          \
    }
#define NULLNEXUS_VALIDWS (TCPWS && TCPWS->is_open())

class WebSocketClient {
	// Settings
	std::string Host, Port, Endpoint;
	std::vector<std::pair<std::string, std::string>> CustomConnectHeaders;
	// Message callback
	std::function<void(std::string)> Callback;

	// ASIO
	net::io_context IOC;
	std::optional<net::executor_work_guard<decltype(IOC.get_executor())>> Work;
	std::optional<websocket::stream<tcp::socket>> TCPWS;
	beast::flat_buffer Buf;

	// Delayed start (after failed connect)
	net::deadline_timer start_delay_timer = net::deadline_timer(IOC);
	// Message list with delivery when connected
	net::deadline_timer MessageQueueTimer = net::deadline_timer(IOC);
	std::queue<std::string> Messages;

	// Worker thread
	std::optional<std::thread> Worker;

	bool IsRunning = false;

	void Log(std::string msg)
	{
		// std::cout << msg << std::endl;
	}

	void HandleHandlerError(const boost::system::error_code& ec)
	{
		if (ec == net::error::basic_errors::operation_aborted) { return; }
		Log(ec.message() + " " + std::to_string(ec.value()));
		ScheduleDelayedStart();
	}

	// Function to run the internal ASIO loop
	void RunIO()
	{
		IOC.run();
		Log("IOC exited");
	}

	// Function gets called whenever a message or error is sent
	void HandlerOnRead(const boost::system::error_code& ec, std::size_t)
	{
		if (ec)
		{
			// Let someone else handle this error
			HandleHandlerError(ec);
			return;
		}
		// Send message to callback
		Callback(beast::buffers_to_string(Buf.data()));
		Buf.clear();
		// we stop reading after this call. We need to restart the handler.
		StartAsyncRead();
	}

	void HandlerOnTCPConnect(const boost::system::error_code& ec, std::promise<void>* ret)
	{
		if (ec)
		{
			Log("Connection to server failed!");
			// Something is waiting for the first connection attempt to finish
			if (ret) { ret->set_value(); }
			if (ec == net::error::basic_errors::operation_aborted) { return; }
			ScheduleDelayedStart();
			return;
		}
		DoWebsocketSetup(ret);
	}

	// Start async reading from ASIO websocket
	void StartAsyncRead()
	{
		NULLNEXUS_GETWS(async_read(Buf, beast::bind_front_handler(&WebSocketClient::HandlerOnRead, this)))
	}

	void DoWebsocketSetup(std::promise<void>* ret)
	{
		try
		{
			// Set a decorator to change the User-Agent of the handshake
			NULLNEXUS_GETWS(set_option(websocket::stream_base::decorator([&](websocket::request_type &req) {
				for (auto &entry : CustomConnectHeaders)
				{
				req.set(entry.first, entry.second);
				}
				req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
				})));
			// Perform the websocket handshake
			NULLNEXUS_GETWS(handshake(Host, Endpoint))

			Log("CO: Connected to the server.");
			// Something is waiting for the first connection attempt to finish
			if (ret) { ret->set_value(); }

			StartAsyncRead();
			// Send cached messages
			TrySendMessageQueue();
		}
		catch (...)
		{
			// Some error. Trying again later.
			Log("CO: Websocket setup failed!");
			// Something is waiting for the first connection attempt to finish
			if (ret) { ret->set_value(); }
			ScheduleDelayedStart();
		}
	}

	// Called by internalStart to run the actual connection code
	void DoConnectionAttempt(std::promise<void>* ret = nullptr)
	{
		try
		{
			tcp::resolver resolver{IOC};

			// Look up the domain name
			const auto results = resolver.resolve(Host, Port);

			// Create a new websocket, old one can't be used anymore after a .close() call
			TCPWS.emplace(IOC);

			net::async_connect(TCPWS->next_layer(), results.begin(), results.end(),
			                   std::bind(&WebSocketClient::HandlerOnTCPConnect, this, std::placeholders::_1, ret));
		}
		catch (...)
		{
			// Some error. Trying again later.
			Log("CO: Connection to server failed!");
			// Something is waiting for the first connection attempt to finish
			if (ret) { ret->set_value(); }
			ScheduleDelayedStart();
		}
	}

	/* Functions for handling the sending of messages */
	void HandleTimerMessageQueue(const boost::system::error_code& ec)
	{
		if (ec) { return; }
		TrySendMessageQueue();
	}

	void TrySendMessageQueue()
	{
		if (!NULLNEXUS_VALIDWS) { return; }

		while (!Messages.empty())
		{
			try
			{
				if (!NULLNEXUS_VALIDWS) { throw std::exception(); }
				NULLNEXUS_GETWS(write(net::buffer(Messages.front())));
				Messages.pop();
			}
			catch (...)
			{
				MessageQueueTimer.cancel();
				MessageQueueTimer.expires_from_now(boost::posix_time::seconds(1));
				MessageQueueTimer.async_wait([this](auto&& ph1) {
					HandleTimerMessageQueue(std::forward<decltype(ph1)>(ph1));
				});
				return;
			}
		}
	}

	void OnImmediateMessageSend(std::string msg, std::promise<bool>& ret)
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
		catch (...) { ret.set_value(false); }
	}

	void OnAsyncMessageSend(const std::string& msg)
	{
		// Push into a queue
		Messages.push(msg);
		// Try to send said queue
		TrySendMessageQueue();
	}

	/* ~Functions for handling the sending of messages~ */

	// React to the timer being activated
	void HandlerStartDelayTimer(const boost::system::error_code& ec)
	{
		if (ec) { return; }
		DoConnectionAttempt();
	}

	// Use the io_context+worker to sheudule a restart/start
	void ScheduleDelayedStart()
	{
		start_delay_timer.cancel();
		start_delay_timer.expires_from_now(boost::posix_time::seconds(RESTART_WAIT_TIME));
		start_delay_timer.async_wait([this](auto&& ph1) { HandlerStartDelayTimer(std::forward<decltype(ph1)>(ph1)); });
	}

	void InternalStart(std::promise<void>* ret)
	{
		if (IsRunning)
		{
			if (ret) { ret->set_value(); }
			return;
		}
		IsRunning = true;
		DoConnectionAttempt(ret);
	}

	void InternalStop(std::promise<void>& ret)
	{
		if (!IsRunning)
		{
			ret.set_value();
			return;
		}
		IsRunning = false;
		start_delay_timer.cancel();
		// Stop message queue from running while stopped
		MessageQueueTimer.cancel();
		if (TCPWS) { TCPWS->next_layer().cancel(); }
		if (NULLNEXUS_VALIDWS) { NULLNEXUS_GETWS(close(websocket::close_code::normal)); }
		ret.set_value();
	}

	void InternalSetCustomHeaders(std::vector<std::pair<std::string, std::string>> headers, std::promise<void>& ret)
	{
		CustomConnectHeaders = std::move(headers);
		ret.set_value();
	}

public:
	void Start(bool async = false)
	{
		if (async)
		{
			// Let boost deal with anything related to thread safety
			post(IOC, [this] { InternalStart(nullptr); });
		}
		else
		{
			std::promise<void> ret;
			const auto future = ret.get_future();
			// Let boost deal with anything related to thread safety
			post(IOC, [this, capture0 = &ret] { InternalStart(capture0); });
			future.wait();
		}
	}

	void Stop()
	{
		std::promise<void> ret;
		const auto future = ret.get_future();
		// Let boost deal with anything related to thread safety
		post(IOC, [this, &ret] { InternalStop(ret); });
		future.wait();
	}

	bool SendMessage(const std::string& msg, bool sendIfOffline = false)
	{
		if (sendIfOffline)
		{
			// Let the worker thread handle this safely
			post(IOC, [this, msg] { OnAsyncMessageSend(msg); });
			return true;
		}
		std::promise<bool> ret;
		auto future = ret.get_future();
		// Let the worker thread handle this safely
		post(IOC, [this, msg, &ret] { OnImmediateMessageSend(msg, ret); });
		future.wait();
		return future.get();
	}

	void SetCustomHeaders(const std::vector<std::pair<std::string, std::string>>& headers)
	{
		std::promise<void> ret;
		const auto future = ret.get_future();
		post(IOC, [this, headers, &ret] { InternalSetCustomHeaders(headers, ret); });
		future.wait();
	}

	WebSocketClient(std::string host, std::string port, std::string endpoint,
	                std::function<void(std::string)> callback) : Host(std::move(host)), Port(std::move(port)),
														Endpoint(std::move(endpoint)), Callback(std::move(callback))
	{
		Work.emplace(IOC.get_executor());
		Worker.emplace([this] { RunIO(); });
	}

	~WebSocketClient()
	{
		Stop();
		Work.reset();
		Worker->join();
	}
};

#undef NULLNEXUS_GETWS
#undef NULLNEXUS_VALIDWS
