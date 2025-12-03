
#include <command_handler.hpp>
#include <client_commands.hpp>
#include <server_commands.hpp>
#include <rsa.hpp>
#include <server.hpp>
#include <server_hooks.hpp>
#include <utils.hpp>

#include <algorithm>
#include <ctime>
#include <sstream>

#include <gmp.h>

// VERSION 1.0

class BasicListener : public Server::IServerHooks {
public:
	BasicListener () {

		key_pair = rsa::genKeyPairs (2048);

		m_client_handler.RegisterCommand ("SEND_KEY", 		client_send_key);
		m_client_handler.RegisterCommand ("SEND_KEY_PROOF", 	client_send_key_proof);
		
		m_client_handler.RegisterCommand ("STOP", 		client_stop);
		m_client_handler.RegisterCommand ("VOTE", 		client_vote);
		m_client_handler.RegisterCommand ("LOGIN", 		client_login);
		
		m_client_handler.RegisterCommand ("VOTE_BEGIN", 	vote_begin);


		m_server_handler.RegisterCommand ("STOP", server_stop);
		m_server_handler.RegisterCommand ("SEND", server_send);
		m_server_handler.RegisterCommand ("DISCONNECT", server_disconnect);
		m_server_handler.RegisterCommand ("LIST", server_list);
	}

private:

	void OnRequest (Server::Server & server, int client_socket, std::string & request) override {
		Client & client = m_clients.at (client_socket);

		const auto & [name, tokens] = m_client_handler.GetTokens (request);

		m_client_handler.Handle (name, tokens, {server, m_clients.at (client_socket), m_vote});		
	}

	void OnServerRequest (Server::Server & server, std::string & request) override {
		const auto & [name, tokens] = m_client_handler.GetTokens (request);

		m_server_handler.Handle (name, tokens, {server});
	};

	void OnUpdate (Server::Server & server, int client_socket) override {
		Client & client = m_clients.at (client_socket);

		time (&client.last_activity);
	}

	void OnConnect (Server::Server & server, int client_socket) override {
		m_clients.emplace (client_socket, Client {client_socket});

		server.Send (client_socket, StrArgs ("SEND_KEY #1#", key_pair[0]->get_str ()));
		Server::Log ("Client #1# connected.", client_socket);

	}

	void OnDisconnect (Server::Server & server, int client_socket) override {
		m_clients.erase (client_socket);

		Server::Log ("Client #1# disconnected.", client_socket);
	}

	Vote   m_vote;

	pair_t * key_pair;

	std::unordered_map<int, Client> m_clients;

	Server::CommandHandler<CommandContext> m_client_handler;
	Server::CommandHandler<ServerCommandContext> m_server_handler;
};

int main (int argc, char **argv)
{
	char* port = nullptr;
	const char* default_port = "12345";
	
	if (argc == 2) port = argv [1];
	else port = (char *) default_port;

	Server::Server server {new BasicListener {}};

	server.Start ();
}
