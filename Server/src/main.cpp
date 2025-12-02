
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


class BasicListener : public Server::IServerHooks {
public:
	BasicListener () :
		pub_key {0} {

		m_client_handler.RegisterCommand ("VOTE_BEGIN", vote_begin);
		m_client_handler.RegisterCommand ("STOP", client_stop);
		m_client_handler.RegisterCommand ("VOTE", client_vote);
		m_client_handler.RegisterCommand ("LOGIN", client_login);

		m_server_handler.RegisterCommand ("STOP", server_stop);
	}

private:

	void OnRequest (Server::Server & server, int client_socket, std::string & request) override {
		Client & client = m_clients.at (client_socket);

		switch (client.status) {
		case ConnectionState::NO_AUTH:
			// TODO : check si la clef est de la bonne taille

			/*
			if (!StringToBINT (request, client.pub_key)) {
				server.Send (client_socket, "RETURN CODE _ E7");
			}
			*/
			
			// client.pub_key = rsa_decrypt (client.pub_key, prv_key);
			client.status  = ConnectionState::PUB_KEY_KNOWN;

			Server::Log ("Received pub_key {C:GOLD}#1#{} from client {C:GREEN}#2#{}.", client.pub_key.get_str (), client_socket);
			// TODO : select n from 0 to N pseudo randomly doesnt need to be that complicated
			client.pub_key_proof = 0;
			
			//server.Send (client_socket, StrArgs ("#1#", rsa_encrypt (client.pub_key_proof)));
			Server::Log ("Waiting for key confirmation for client {C:GREEN}#1#{}.", client_socket);

			break;
		
		case ConnectionState::PUB_KEY_KNOWN:
			int n_p;
			//n_p = rsa_decrypt (client.pub_key, prv_key);
				
			if (n_p != client.pub_key_proof + 1) {
				server.Send (client_socket, "RETURN CODE _ E8");
				client.status = ConnectionState::NO_AUTH;
			}

			client.status = ConnectionState::PUB_KEY_CONFIRMED;

			break;

		default:
			const auto & [name, tokens] = m_client_handler.GetTokens (request);

			m_client_handler.Handle (name, tokens, {server, m_clients.at (client_socket), m_vote});

			break;
		}
		
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

		server.Send (client_socket, StrArgs ("#1#", pub_key.get_str ()));
		server.Send (client_socket, StrArgs ("connected as client: #1#", client_socket));
		Server::Log ("Client #1# connected.", client_socket);

	}

	void OnDisconnect (Server::Server & server, int client_socket) override {
		m_clients.erase (client_socket);

		Server::Log ("Client #1# disconnected.", client_socket);
	}

	Vote   m_vote;

	mpz_class pub_key;
	mpz_class prv_key;

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
