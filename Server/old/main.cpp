
#include <utils.hpp>
#include <server_interface.hpp>

#include <algorithm>
#include <ctime>
#include <sstream>

using BigInt = __uint128_t;

enum class ConnectionState {
	NONE,
	KNOWN_PUB_KEY,
	LOGIN
};

struct Client {
	time_t	last_activity;
	bool	is_admin;
	BigInt	pub_key;

	Client () : last_activity (-1), is_admin (false), pub_key (0) {

	}
};

class ServerHandler : public IHandler {
public:
	ServerHandler () {

	}

private:
	void OnRequest (Server & server, int client_socket, char * request) override {
		
	}

	void OnUpdate (Server & server, int client_socket) override {
		time_t timestamp;
		Client & client = m_clients.at (client_socket);


		time (&timestamp);
		client.last_activity = timestamp;
	}

	void OnConnect (Server & server, int client_socket) override {
		m_clients.insert ({client_socket, Client {}});

		server.Send (client_socket, "Hello World");
		Log ("Client #1# connected.", client_socket);

	}

	void OnDisconnect (Server & server, int client_socket) override {
		m_clients.erase (client_socket);

		Log ("Client #1# disconnected.", client_socket);
	}

	BigInt pub_key;
	BigInt prv_key;

	std::unordered_map<int, Client> m_clients;
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