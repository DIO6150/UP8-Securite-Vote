
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
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <thread>
#include <vector>

#include <client.hpp>
#include <command_status.hpp>
#include <string_pretty.hpp>
#include <utils.hpp>

#define TIMEOUT_SUS 		15
#define TIMEOUT_KILL 		30
#define CHECK_INACTIVITY	true

template<class ClientProperties>
class Server {
	using server_callback = std::function<void (std::vector<std::string>, Client<ClientProperties> &, Server<ClientProperties> * server)>;

	public:
	Server (char *port) : m_should_run {true} {
		m_unknown_command = fun;
		m_connection = fun;
		m_disconnection = fun;
		m_echo = fun;


		struct addrinfo 		hints, *gai, *ai;
		int				err;
		int				yes;

		yes = 1;
		
		bzero (&hints, sizeof (hints));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if ((err = getaddrinfo (NULL, port, &hints, &gai)) < 0) {
			Log ("{C:RED}ERROR : getaddrinfo {C:MAGENTA}#1#{}", gai_strerror (err));
			exit (EXIT_FAILURE);
		}

		for (ai = gai; ai != NULL; ai = ai->ai_next) {
			if ((m_socket = socket (ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
				Log ("{C:RED}ERROR : socket{}");
				continue;
			}
			
			if (setsockopt (m_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) < 0) {
				Log ("{C:RED}ERROR : setsockopt{}");
				exit (EXIT_FAILURE);
			}

			if (bind (m_socket, ai->ai_addr, ai->ai_addrlen) < 0) {
				close (m_socket);
				Log ("{C:RED}ERROR : bind{}");
				continue;
			}

			break;
		}

		freeaddrinfo (gai);

		if (ai == NULL) {
			Log ("Failed to bind server");
			exit (EXIT_FAILURE);
		}

		m_id_counter = 0;

		Log ("Server created on port: #1#", port);
	}

	Server () : Server {(char *)"12345"} {

	}

	~Server () {
		if (m_socket) close (m_socket);
	}

	void Send (Client<ClientProperties> & client, std::string message) {
		if (message == "") return;

		message += "\n";

		if (write (client.GetSocket (), message.c_str (), message.length ()) <= 0) {
			// TODO : error handling
			return;
		}
	}

	void Broadcast (std::string message) {
		for (auto & [n, client] : m_clients) {
			if (write (client.GetSocket (), message.c_str (), message.length ()) <= 0) {
				// TODO : error handling
				continue;
			}
		}
	}
	
	template<class ... Args>
	void Log (const std::string & message, Args ... args) {
		std::cerr << "[SERVER] " << StrPretty (message, args ...) << "\n";
	}

	void Start () {
		char 				buffer[1024];
		int 				err;

		sockaddr_storage		from;
		int				len;

		time_t 				timestamp;

		int 				client_fd;
		Client<ClientProperties> *	client;

		len = sizeof(from);

		if (listen (m_socket, 8) < 0) {
			exit (EXIT_FAILURE);
		}

		std::thread listen_thread {[this]() { this->Listen (); }};

		Log ("Server is now running");

		while (m_should_run) {
			m_mutex.lock ();

			poll (m_observers.data (), m_observers.size (), 0);

			for (int i = 0; i < m_observers.size (); ++i) {
				client_fd 	=   m_observers[i].fd;
				client 		= & m_clients[client_fd];

				time (&timestamp);

				/*
				if (timestamp - client->GetLastActivity () >= TIMEOUT_SUS && !client->IsSuspious ()) {
					client->SetDeadSuspicionFlag (true);
					Send (*client, "ALIVE");
				}
				*/

				if (CHECK_INACTIVITY && timestamp - client->GetLastActivity () >= TIMEOUT_KILL && client->IsSuspious ()) {
					UnregisterClient (client_fd);
					--i; // should be fine I think (its because we swap-and-pop m_observers [i] so technically the old .back is now at index i)
					continue;
				}

				if (client->IsDead ()) {
					UnregisterClient (client_fd);
					--i; // should be fine I think (its because we swap-and-pop m_observers [i] so technically the old .back is now at index i)
					continue;
				}

				if (m_observers [i].revents & POLLIN) {
					bzero (buffer, 1024);

					if ((err = read (client_fd, buffer, sizeof (buffer))) <= 0) {
						if (!err) {
							UnregisterClient (client_fd);
							--i; // should be fine I think (its because we swap-and-pop m_observers [i] so technically the old .back is now at index i)
							continue;
						}

						UnregisterClient (client_fd);
						--i;
						continue;
					}
					
					HandleClient (*client, std::string {buffer});
				}
			}
			m_mutex.unlock ();
		}
		
		Log ("Shutting Down ...");
		shutdown (m_socket, SHUT_RDWR);
		listen_thread.join ();

		for (auto & [n, client] : m_clients) {
			close (client.GetSocket ());
		}
	}

	void Stop () {
		m_should_run = false;
	}

	void SetCommand (const std::string & name, server_callback fun) {
		m_requests.insert ({name, fun});
	}

	void SetConnectionCommand (server_callback fun) {
		m_connection = fun;
	}

	void SetDisconnectionCommand (server_callback fun) {
		m_disconnection = fun;
	}

	void SetUnknownCommand (server_callback fun) {
		m_unknown_command = fun;
	}

	void SetEchoCommand (server_callback fun) {
		m_echo = fun;
	}


private:
	bool							m_should_run;

	char *							m_port;
	int							m_socket;

	std::vector<pollfd>					m_observers;
	std::unordered_map<int, Client<ClientProperties>>	m_clients;

	std::mutex						m_mutex;

	int							m_id_counter;

	std::unordered_map<std::string, server_callback> 	m_requests;

	server_callback			 			m_unknown_command;
	server_callback			 			m_connection;
	server_callback			 			m_disconnection;
	server_callback			 			m_echo;

	server_callback 					fun = [] (std::vector<std::string> args, Client<ClientProperties> & client, Server<ClientProperties> * server) {
		server->Log ("Command Undefined, fallback function.");
	};


	/*
		WARNNING : Must lock `m_clients_mutex` and `m_observer_mutex` before using this method
	*/
	Client<ClientProperties> & RegisterClient (int fd, Client<ClientProperties> data) {
		if (m_clients.find (fd) != m_clients.end ()) {
			Log ("Client could not be registered as its file descriptor is already registered.");
			exit (EXIT_FAILURE);
		}

		m_observers.push_back ({fd, POLLIN});
		m_clients.insert ({fd, data});

		return (m_clients [fd]);
	}

	void UnregisterClient (int fd) {
		if (m_clients.find (fd) == m_clients.end ()) {
			Log ("Client cannot be removed as its file descriptor is not registered.");
		}

		Client<ClientProperties> * client = &m_clients [fd];

		m_disconnection ({}, *client, this);

		m_clients.erase (fd);

		close (fd);

		for (int i = 0; i < m_observers.size (); ++i) {
			if (m_observers [i].fd == fd) {
				m_observers [i] = m_observers.back ();
				m_observers.pop_back ();
				return;
			}
		}
	}

	void HandleClient (Client<ClientProperties> & client, std::string request) {
		client.UpdateActivity ();

		trim (request);
		if (request == "") return;

		std::vector<std::string> buffered = split (request, "\n");

		for (auto & r : buffered) {
			auto tokens = split (r, " ");
			auto pos = m_requests.find (tokens[0]);

			client.UpdateCommandStats (r, tokens, pos == m_requests.end () ? CommandStatus::INVALID : CommandStatus::VALID);
			m_echo			(tokens, client, this);

			if (client.GetCommandStatus () == CommandStatus::VALID) {
				pos->second (tokens, client, this);
			}
			else {
				m_unknown_command (tokens, client, this);
			}
		}
	}

	void Listen () {
		sockaddr_storage			from;
		int					len;
		int					client_socket;

		len = sizeof(from);

		while (m_should_run) {
			if ((client_socket = accept (m_socket, (sockaddr *) &from, (socklen_t *) &len)) < 0) {
				Log ("[LISTENER] Something went wrong.", client_socket);
			}

			if (client_socket > 0) {
				std::lock_guard<std::mutex> guard_c {m_mutex};

				Client<ClientProperties> &client = RegisterClient (client_socket, {m_id_counter++, client_socket});

				m_connection ({}, client, this);
			}
		}

		Log ("[LISTENER] Listening Thread Shutting Down");
	}

	server_callback & GetCommand (const std::string & name) {
		typename std::unordered_map<std::string, server_callback>::iterator pos;
		if ((pos = m_requests.find (name)) == m_requests.end ()) return (m_unknown_command);

		return (pos->second);
	}
};