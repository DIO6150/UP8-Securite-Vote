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
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class Server;

class IHandler {
public:	
	virtual void OnRequest	 	(Server & server, int client_socket, char * buffer) = 0;
	virtual void OnUpdate 		(Server & server, int client_socket) = 0;
	virtual void OnConnect 		(Server & server, int client_socket) = 0;
	virtual void OnDisconnect 	(Server & server, int client_socket) = 0;
};

class Server {
public:
	Server (IHandler * handler);
	Server (const char * port, IHandler * handler);

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
	IHandler *		m_handler;

	std::vector<pollfd>	m_observers;

	std::deque<int>		m_connection_queue;
	std::deque<int>		m_disconnection_queue;

	std::mutex		m_run_mutex;
	std::mutex		m_connection_queue_mutex;
	
	void Listen ();
};