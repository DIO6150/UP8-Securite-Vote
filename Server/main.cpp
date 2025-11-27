
#include <utils.hpp>
#include <server_hooks.hpp>
#include <server.hpp>
#include <command_handler.hpp>
#include <rsa.hpp>

#include <algorithm>
#include <ctime>
#include <sstream>

enum class ConnectionState : int {
	NO_AUTH,
	PUB_KEY_KNOWN,
	PUB_KEY_CONFIRMED,
	FULL_AUTH
};

struct Client {
	int 		socket;
	bool		is_admin;

	time_t		last_activity;
	BigInt		pub_key;

	ConnectionState	status;

	int		pub_key_proof;

	Client (int socket) : 
		socket {socket}, 
		is_admin {false}, 
		last_activity {-1}, 
		pub_key {0},
		status {ConnectionState::NO_AUTH} {

	}

	Client () : Client (0) {

	}

	bool IsAuth () {
		return (status == ConnectionState::FULL_AUTH);
	}

	bool IsAdmin () {
		return (is_admin && IsAuth ());
	}

	bool IsMidAuth () {
		return (status == ConnectionState::PUB_KEY_CONFIRMED);
	}
};

struct Vote {
	std::vector<std::string>	candidates;
};

struct CommandContext {
	Server & server;
	Client & client;
	Vote   & vote;
};

struct ServerCommandContext {
	Server & server;
};

class BasicListener : public IServerHooks {
public:
	BasicListener () :
		pub_key {0} {
		m_client_handler.RegisterCommand ("STOP", [](const std::vector<std::string> & args, CommandContext & context) {
			if (context.client.IsAdmin ()) {
				context.server.Stop ();
			}
		});

		m_client_handler.RegisterCommand ("VOTE", [](const std::vector<std::string> & args, CommandContext & context) {
			if (!context.client.IsAuth ()) {
				context.server.Send (context.client.socket, "ERROR CODE VOTE E1");
				return;
			}

			size_t n_candidate = args.size ();

			
		});

		m_client_handler.RegisterCommand ("LOGIN", [] (const std::vector<std::string> & args, CommandContext & context) {
			if (context.client.status == ConnectionState::FULL_AUTH) {
				context.server.Send (context.client.socket, "RETURN CODE LOGIN E9");
				return;
			}

			// TODO : tester si le nom d'utilisateur existe bien
			bool status = true;

			if (!status) {
				context.server.Send (context.client.socket, "RETURN CODE LOGIN E2");
			}

			// TODO: tester si le mot de passe associer à l'utilisateur est le bon
			status = true;

			if (!status) {
				context.server.Send (context.client.socket, "RETURN CODE LOGIN E3");
			}

			context.server.Send (context.client.socket, "RETURN CODE LOGIN O0");
			context.client.status = ConnectionState::FULL_AUTH;
		});

		m_server_handler.RegisterCommand ("STOP", [](const std::vector<std::string> & args, ServerCommandContext & context) {
			context.server.Stop ();
		});
	}

private:

	void OnRequest (Server & server, int client_socket, std::string & request) override {
		Client & client = m_clients.at (client_socket);

		switch (client.status) {
		case ConnectionState::NO_AUTH:
			// TODO : check si la clef est de la bonne taille

			if (!StringToBINT (request, client.pub_key)) {
				server.Send (client_socket, "RETURN CODE _ E7");
			}
			
			client.pub_key = rsa_decrypt (client.pub_key, prv_key);
			client.status  = ConnectionState::PUB_KEY_KNOWN;

			Log ("Received pub_key {C:GOLD}#1#{}from client {C:GREEN}#2#{}.", client.pub_key, client_socket);
			// TODO : select n from 0 to N pseudo randomly doesnt need to be that complicated
			client.pub_key_proof = 0;
			
			server.Send (client_socket, StrArgs ("#1#", rsa_encrypt (client.pub_key_proof)));
			Log ("Waiting for key confirmation for client {C:GREEN}#1#{}.", client_socket);

			break;
		
		case ConnectionState::PUB_KEY_KNOWN:
			int n_p = rsa_decrypt (client.pub_key, prv_key);
				
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

	void OnServerRequest (Server & server, std::string & request) override {
		const auto & [name, tokens] = m_client_handler.GetTokens (request);

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

	Vote   m_vote;

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

	Server server {new BasicListener {}};

	server.Start ();
}