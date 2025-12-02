#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Server {
	class IServerHooks;

	class Server {
	public:
		Server (IServerHooks *&& handler);
		Server (const char * port, IServerHooks *&& handler);

		~Server ();

		Server (const Server & other) = delete;
		Server (Server && other) = delete;

		Server & operator= (const Server & other) = delete;
		Server & operator= (Server && other) = delete;

		void Start ();
		void Stop ();

		void Broadcast (std::string message);
		void Send (int client, std::string message);

		void Disconnect (int client);
		void ForceDisconnect (int client);

	private:
		int			m_server;
		bool			m_run;
		IServerHooks *		m_handler;

		std::vector<pollfd>		m_observers;
		std::unordered_map<int, int> 	m_observers_index; // client_socket -> m_observer index

		std::deque<int>		m_connection_queue;
		std::set  <int>		m_disconnection;

		std::mutex		m_run_mutex;
		std::mutex		m_connection_queue_mutex;
		
		void Listen ();

		void AddClient (int fd);
		void RemoveClient (int fd);

		bool HeaderHandler 	(int & client_index, int bytes, int client_fd);
		bool BodyHandler 	(int & client_index, int bytes, int body_size, int client_fd, char * body_buffer);
	};
}