
#include <utils.hpp>
#include <server_interface.hpp>
#include <command_handler.hpp>

#include <algorithm>
#include <ctime>
#include <sstream>

using BigInt = uint64_t;

enum class ConnectionState {
	NONE,
	KNOWN_PUB_KEY,
	LOGIN
};

struct Client {
	int 	socket;
	bool	is_admin;

	time_t	last_activity;
	BigInt	pub_key;

	Client (int socket) : 
		socket (socket), 
		is_admin (false), 
		last_activity (-1), 
		pub_key (0) {

	}

	Client () : Client (0) {

	}
};

struct CommandContext {
	Server & server;
	Client & client;
};

struct ServerCommandContext {
	Server & server;
};

class ServerHandler : public IHandler {
public:
	ServerHandler () :
		pub_key {0} {
		m_client_handler.RegisterCommand ("STOP", [](const std::vector<std::string> & args, CommandContext & context) {
			if (context.client.is_admin) {
				context.server.Stop ();
			}
		});

		m_client_handler.RegisterCommand ("ECHO", [](const std::vector<std::string> & args, CommandContext & context) {
			for (const auto & arg : args) {
				context.server.Send (context.client.socket, arg);
			}
		});

		m_server_handler.RegisterCommand ("STOP", [](const std::vector<std::string> & args, ServerCommandContext & context) {
			context.server.Stop ();
		});
	}

private:

	void OnRequest (Server & server, int client_socket, std::string & request) override {
		auto tokens = m_client_handler.GetTokens (request);
		const auto name = tokens.front ();
		tokens.erase (tokens.begin ());

		Log ("Client [#1#] > {C:GOLD}#2#", client_socket, request);
		
		m_client_handler.Handle (name, tokens, {server, m_clients.at (client_socket)});
	}

	void OnServerRequest (Server & server, std::string & request) override {
		auto tokens = m_client_handler.GetTokens (request);
		const auto name = *tokens.erase (tokens.begin ());

		m_server_handler.Handle (name, tokens, {server});
	};

	void OnUpdate (Server & server, int client_socket) override {
		Client & client = m_clients.at (client_socket);

		time (&client.last_activity);
	}

	void OnConnect (Server & server, int client_socket) override {
		m_clients.emplace (client_socket, Client {client_socket});

		server.Send (client_socket, StrArgs ("#1#", pub_key));
		server.Send (client_socket, StrArgs ("connected as client: #1#", client_socket));
		Log ("Client #1# connected.", client_socket);

	}

	void OnDisconnect (Server & server, int client_socket) override {
		m_clients.erase (client_socket);

		Log ("Client #1# disconnected.", client_socket);
	}

	BigInt pub_key;
	BigInt prv_key;

	std::unordered_map<int, Client> m_clients;

	CommandHandler<CommandContext> m_client_handler;
	CommandHandler<ServerCommandContext> m_server_handler;
};

int main (int argc, char **argv)
{
	char* port = nullptr;
	const char* default_port = "12345";
	
	if (argc == 2) port = argv [1];
	else port = (char *) default_port;

	Server server {new ServerHandler {}};

	server.Start ();
}