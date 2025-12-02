#pragma once

#include <string>

namespace Server {
	class Server;

	class IServerHooks {
	public:
		virtual void OnRequest	 	(Server & server, int client_socket, std::string & request) = 0;
		virtual void OnServerRequest 	(Server & server, std::string & request) = 0;
		virtual void OnUpdate 		(Server & server, int client_socket) = 0;
		virtual void OnConnect 		(Server & server, int client_socket) = 0;
		virtual void OnDisconnect 	(Server & server, int client_socket) = 0;
	};
}