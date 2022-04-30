/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "WebSocketClient.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <optional>

// Reference implementation of a cheat-agnostic nullnexus client
class NullNexus {
public:
	// TF2 Server info + steamid, used for authenticating with other nullnexus users
	struct TF2Server {
		bool Connected;
		std::string IP;
		std::string Port;
		std::string SteamID;
		int ServerSpawnCount;

		bool operator==(const TF2Server& other)
		{
			return Connected == other.Connected && IP == other.IP && Port == other.Port && SteamID == other.SteamID &&
				ServerSpawnCount == other.ServerSpawnCount;
		}

		TF2Server(bool connected = false, std::string ip = "", std::string port = "", std::string steamid = "", int server_spawn_count = -1) : Connected(connected), IP(ip), Port(port), SteamID(steamid), ServerSpawnCount(server_spawn_count)
		{
		}
	};

	struct UserSettings {
		std::optional<std::string> Username;
		std::optional<int> Color;
		std::optional<TF2Server> GameServer;
	};

private:
	std::unique_ptr<WebSocketClient> WSClient;
	// Are settings set up yet?
	bool SettingsSet = false;
	UserSettings Settings;

	// Callbacks
	std::optional<std::function<bool(boost::property_tree::ptree tree)>> CallbackCustom;
	std::optional<std::function<void(std::string username, std::string message, int colour)>> CallbackChat;
	std::optional<std::function<void(std::vector<std::string> steamids)>> CallbackAuthedPlayers;

	bool SendAuthenticatedMessage(bool reliable, const std::string& type, const boost::property_tree::ptree& child)
	{
		if (!WSClient) { return false; }
		boost::property_tree::ptree pt;
		// Basic data
		pt.put("username", *Settings.Username);
		pt.put("type", type);

		// Data exclusive to this request
		pt.put_child("data", child);

		std::ostringstream buf;
		write_json(buf, pt, false);
		return WSClient->SendMessage(buf.str(), reliable);
	}

	void HandleMessageChat(boost::property_tree::ptree& tree)
	{
		const auto& data = tree.get_child("data");
		if (CallbackChat)
		{
			(*CallbackChat)(data.get<std::string>("user"), data.get<std::string>("msg"), data.get<int>("colour"));
		}
	}

	void HandleMessageAuthedPlayers(boost::property_tree::ptree& tree)
	{
		if (!CallbackAuthedPlayers) { return; }
		std::vector<std::string> steamids;
		for (auto& item : tree.get_child("data")) { steamids.push_back(item.second.get<std::string>("steamid")); }
		(*CallbackAuthedPlayers)(steamids);
	}

	void HandleMessage(const std::string& msg)
	{
		try
		{
			// Parse message
			std::istringstream iss(msg);
			boost::property_tree::ptree pt;
			read_json(iss, pt);

			if (CallbackCustom)
			{
				// If the custom callback handled this message, we should stop
				if ((*CallbackCustom)(pt)) { return; }
			}

			const auto type = pt.get<std::string>("type");
			if (type == "chat") { HandleMessageChat(pt); }
			else if (type == "authedplayers") { HandleMessageAuthedPlayers(pt); }
		}
		catch (...) { }
	}

	void SetCustomHeaders()
	{
		std::vector<std::pair<std::string, std::string>> headers = {
			{"nullnexus_colour", std::to_string(*Settings.Color)}
		};
		if (Settings.GameServer && Settings.GameServer->Connected)
		{
			headers.emplace_back("nullnexus_server_ip", Settings.GameServer->IP);
			headers.emplace_back("nullnexus_server_port", Settings.GameServer->Port);
			headers.emplace_back("nullnexus_server_steamid", Settings.GameServer->SteamID);
			headers.emplace_back("nullnexus_server_server_spawn_count", std::to_string(Settings.GameServer->ServerSpawnCount));
		}
		if (WSClient) { WSClient->SetCustomHeaders(headers); }
	}

public:
	UserSettings GetSettings()
	{
		return Settings;
	}

	// Change some setting
	void ChangeData(UserSettings newSettings = UserSettings())
	{
		SettingsSet = true;
		{
			if ((!Settings.Username && !newSettings.Username) || (newSettings.Username && *newSettings.Username ==
				"anon")) { Settings.Username = "Anon-" + std::to_string(rand() % 9000 + 1000); }
			else if (!Settings.Username || (newSettings.Username && *newSettings.Username != *Settings.Username))
			{
				Settings.Username = *newSettings.Username;
			}
		}

		// RNG colour generator
		if (!newSettings.Color && !Settings.Color)
		{
			const int r = (rand() % 255 + 255) / 2;
			const int g = (rand() % 255 + 255) / 2;
			const int b = (rand() % 255 + 255) / 2;
			newSettings.Color = (r << 16) + (g << 8) + b;
		}
		
		// Set colour
		boost::property_tree::ptree pt;
		if (newSettings.Color && (!Settings.Color || *newSettings.Color != *Settings.Color))
		{
			Settings.Color = *newSettings.Color;
			pt.put("colour", *Settings.Color);
		}

		// Send info about current server to nullnexus instance
		if (newSettings.GameServer && (!Settings.GameServer || !(*newSettings.GameServer == *Settings.GameServer)))
		{
			boost::property_tree::ptree pt_server;
			pt_server.put("connected", newSettings.GameServer->Connected);
			if (newSettings.GameServer->Connected)
			{
				pt_server.put("ip", newSettings.GameServer->IP);
				pt_server.put("port", newSettings.GameServer->Port);
				pt_server.put("steamid", newSettings.GameServer->SteamID);
				pt_server.put("server_spawn_count", std::to_string(Settings.GameServer->ServerSpawnCount));
			}
			Settings.GameServer = *newSettings.GameServer;
			pt.put_child("server", pt_server);
		}

		if (!pt.empty())
		{
			SendAuthenticatedMessage(false, "dataupdate", pt);
			// In case of reconnect
			SetCustomHeaders();
		}
	}

	void Disconnect()
	{
		if (WSClient) { WSClient->Stop(); }
	}

	void Reconnect(bool async = false)
	{
		if (WSClient) { WSClient->Stop(); }
		WSClient->Start(async);
	}

	// Connect to a specific server
	void Connect(const std::string& host = "localhost", const std::string& port = "3000", const std::string& endpoint = "/api/v1/client", bool async = false)
	{
		if (!SettingsSet) { ChangeData(); }
		WSClient = std::make_unique<WebSocketClient>(host, port, endpoint,
		                                             [this](auto&& ph1) {
			                                             HandleMessage(std::forward<decltype(ph1)>(ph1));
		                                             });
		SetCustomHeaders();
		WSClient->Start(async);
	}

	// Send a chat message
	bool SendChat(std::string message, const std::string& location = "public")
	{
		if (message.size() > 128 || message.empty()) { return false; }
		if (std::count_if(message.begin(), message.end(), [](unsigned char c) { return !std::isprint(c); }) != 0)
		{
			return false;
		}

		boost::property_tree::ptree pt;
		pt.put("msg", message);
		pt.put("loc", location);
		return SendAuthenticatedMessage(false, "chat", pt);
	}

	// Add a handler that overrides all other handlers implemented by this class.
	// Return true if your handler handled the message, false if you want the class to handle it.
	void SetHandlerCustom(const std::function<bool(boost::property_tree::ptree tree)>& handler) { CallbackCustom = handler; }

	// Handle chat messages
	void SetHandlerChat(const std::function<void(std::string username, std::string message, int colour)>& handler)
	{
		CallbackChat = handler;
	}

	// Handle authedplayers messages
	void SetHandlerAuthedplayers(const std::function<void(std::vector<std::string>)>& handler)
	{
		CallbackAuthedPlayers = handler;
	}
};
