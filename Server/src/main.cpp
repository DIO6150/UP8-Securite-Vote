
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
	BasicListener () {

		key_pair = rsa::genKeyPairs (2048);

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
		case ConnectionState::NO_AUTH: {
			if (request.size () < (2048 / 8)) {
				Server::Log ("{C:RED}Received invalid pub_key: {C:GOLD}#1#{} from client {C:GREEN}#2#{}. Wrong Length.", client.pub_key.get_str (), client_socket);
				server.Send (client_socket, "RETURN CODE _ E7");
				return;
			}

			if (client.pub_key.set_str (request.substr (0, (2048 / 8)), 10) < 0) {
				server.Send (client_socket, "RETURN CODE _ E7");
				return;
			}
			
			
			// client.pub_key = rsa_decrypt (client.pub_key, prv_key);
			client.status = ConnectionState::PUB_KEY_KNOWN;

			Server::Log ("Received pub_key {C:GOLD}#1#{} from client {C:GREEN}#2#{}.", client.pub_key.get_str (), client_socket);
			// TODO : select n from 0 to N pseudo randomly doesnt need to be that complicated
			client.pub_key_proof = 0;
			
			std::string proof_str = std::to_string (client.pub_key_proof); //StrArgs ("#1#", rsa::code (client.pub_key_proof, &client.pub_key ).get_str ());
			server.Send (client_socket, proof_str);
			Server::Log ("Waiting for key confirmation for client {C:GREEN}#1#{}.", client_socket);

			break;
		}
		
		case ConnectionState::PUB_KEY_KNOWN: {

			int n_p;
			// mpz_class t;
			// t = rsa::code (client.pub_key, key_pair[1]);
			//n_p = t.get_si ();

			n_p = std::stoi (request);
				
			if (n_p != client.pub_key_proof + 1) {
				Server::Log ("{C:BLUE}(DEBUG) Expected proof: #1# ; Proof gotten : #2#", client.pub_key_proof + 1, n_p);
				server.Send (client_socket, "RETURN CODE _ E8");
				client.status = ConnectionState::NO_AUTH;
			}
			
			client.status = ConnectionState::PUB_KEY_CONFIRMED;
			
			break;
		}

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

		server.Send (client_socket, StrArgs ("#1#", key_pair[0]->get_str ()));
		server.Send (client_socket, StrArgs ("(DEBUG) you are: #1#", client_socket));
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
