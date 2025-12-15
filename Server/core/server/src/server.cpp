#include <server.hpp>
#include <server_hooks.hpp>
#include <utils.hpp>

#include <cstring>

#define HEADER_SIZE 4
#define MAX_BODY_SIZE 65536

static void setup_server (const char * port, int & server) {
	struct addrinfo 		hints, *gai, *ai;
	int				err;
	int				yes;

	yes = 1;
	
	bzero (&hints, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((err = getaddrinfo (NULL, port, &hints, &gai)) < 0) {
		Server::Log ("{C:RED}ERROR : getaddrinfo {C:MAGENTA}#1#{}", gai_strerror (err));
		exit (EXIT_FAILURE);
	}

	for (ai = gai; ai != NULL; ai = ai->ai_next) {
		if ((server = socket (ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
			Server::Log ("{C:RED}ERROR : socket{}");
			continue;
		}
		
		if (setsockopt (server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) < 0) {
			Server::Log ("{C:RED}ERROR : setsockopt{}");
			exit (EXIT_FAILURE);
		}

		if (bind (server, ai->ai_addr, ai->ai_addrlen) < 0) {
			close (server);
			Server::Log ("{C:RED}ERROR : bind{}");
			continue;
		}

		break;
	}

	freeaddrinfo (gai);

	if (ai == NULL) {
		Server::Log ("Failed to bind server");
		exit (EXIT_FAILURE);
	}
}

namespace Server {
	Server::Server (IServerHooks *&& handler) : Server {"12345", std::move (handler)} {

	}

	Server::Server (const char * port, IServerHooks *&& handler) :
	m_run (true),
	m_handler (handler) {
		setup_server (port, m_server);
		
		Log ("Server created on port: #1#", port);
	}

	Server::~Server () {
		if (m_server) {
			close (m_server);
		}

		delete m_handler;
	}

	void Server::Start () {
		int			n_bytes;
		
		sockaddr_storage	from;
		int			length;
		
		char			header_buffer[4];
		char *			body_buffer;

		int32_t			body_size;

		pollfd			server_input;

		length = sizeof (from);

		if (listen (m_server, 8) < 0) {
			exit (EXIT_FAILURE);
		}

		std::thread listen_thread {[this]() { this->Listen (); }};

		Log ("Server is now running");

		server_input = pollfd {STDIN_FILENO, POLLIN};

		while (m_run) {
			
			// Server Console
			poll (&server_input, 1, 0);
			if (server_input.revents & POLLIN) {
				std::string request;
				std::getline (std::cin, request);
				m_handler->OnServerRequest (*this, request);
			}


			// Clients Connection Handler
			m_connection_queue_mutex.lock ();
			
			while (!m_connection_queue.empty ()) {
				int client_socket = m_connection_queue.front ();
				m_connection_queue.pop_front ();
				
				AddClient (client_socket);
				m_handler->OnConnect (*this, client_socket);
			}

			m_connection_queue_mutex.unlock ();
			

			// Clients Handler
			poll (m_observers.data (), m_observers.size (), 0);
			for (int client_index = 0; client_index < m_observers.size ();) {
				pollfd client_poll = m_observers[client_index];
				int client_fd = client_poll.fd;
				
				// Disconnection handler
				if (m_disconnection.find (client_fd) != m_disconnection.end ()) {
					close (client_fd);
					m_handler->OnDisconnect (*this, client_fd);
					RemoveClient (client_fd);

					continue;
				}

				// Request Handler
				if (client_poll.revents & POLLIN) {
					m_handler->OnUpdate (*this, client_fd);
					
					
					// Request Header
					bzero (header_buffer, HEADER_SIZE);
					n_bytes = read (client_fd, header_buffer, HEADER_SIZE);
					if (!HeaderHandler (client_index, n_bytes, client_fd)) continue;


					// Request Body
					body_size = *(int*)header_buffer;
					body_size = ntohl (body_size);
					body_size = body_size >= MAX_BODY_SIZE ? MAX_BODY_SIZE : body_size;
					
					body_buffer = (char *) malloc (body_size + 1);
					
					poll (&client_poll, 1, 0);
					if (!(client_poll.revents & POLLIN)) {
						Log ("{C:RED}Error body is of size 0.");
						
						close (client_fd);
						m_handler->OnDisconnect (*this, client_fd);
						RemoveClient (client_fd);
						
						free (body_buffer);
						
						continue;
					}
					
					n_bytes = read (client_fd, body_buffer, body_size);
					if (!BodyHandler (client_index, n_bytes, body_size, client_fd, body_buffer)) continue;
					body_buffer[body_size] = '\0';


					// Request Handler
					std::string request {body_buffer};
					m_handler->OnRequest (*this, client_fd, request);
					free (body_buffer);

				}
				
				++client_index;
			}
		}
		
		Log ("Shutting Down ...");

		shutdown (m_server, SHUT_RDWR);
		listen_thread.join ();
		
		for (const auto & observer : m_observers) {
			close (observer.fd);
		}

		m_observers.clear ();
	}

	void Server::Stop () {
		m_run = false;
	}

	void Server::Broadcast (std::string message) {
		std::for_each (
			m_observers.begin (),
			m_observers.end (),
			[this, &message](pollfd poll) {
				this->Send (poll.fd, message);
			}
		);
	}

	void Server::Send (int client, std::string message) {
		if (message == "") return;

		uint32_t 	header_size;
		char 		header_bytes[4];
		std::string	header;

		header.resize (4);

		header_size = message.length ();

		*(uint32_t *) header_bytes = htonl (header_size);

		header[0] = header_bytes[0];
		header[1] = header_bytes[1];
		header[2] = header_bytes[2];
		header[3] = header_bytes[3];
		
		if (write (client, header_bytes, 4) <= 0) {
			Log ("{C:RED}Error: couldn't send header to client");
			Log ("{C:RED}> #1#", std::strerror (errno));
			return;
		}

		if (write (client, message.c_str (), message.length ()) <= 0) {
			Log ("{C:RED}Error: couldn't send body to client");
			Log ("{C:RED}> #1#", std::strerror (errno));
			return;
		}
	}

	void Server::Disconnect (int client_id) {
		m_disconnection.emplace (client_id);
	}

	void Server::ForceDisconnect (int client_id) {

	}

	bool Server::HasClient (int client) {
		return (m_observers_index.find (client) != m_observers_index.end ());
	}

	std::vector<int> Server::ListClients () const {
		std::vector<int> clients;

		for (const auto & c : m_observers) {
			clients.push_back (c.fd);
		}

		return (clients);
	}

	void Server::Listen () {
		sockaddr_storage	from;
		int			len;
		int			client_socket;

		len = sizeof(from);
		
		for (;;) {
			if ((client_socket = accept (m_server, (sockaddr *) &from, (socklen_t *) &len)) <= 0) {
				if (errno == 22) break;
				Log ("[LISTENER] Something went wrong: #1#.", client_socket);
				Log ("{C:RED}> #1# (#2#)", std::strerror (errno), errno);
				break;
			}
			
			std::lock_guard<std::mutex> queue_guard {m_connection_queue_mutex};
			m_connection_queue.push_back (client_socket);

			if (!m_run) {
				break;
			}
		}

		Log ("[LISTENER] Listening Thread Shutting Down");
	}

	void Server::AddClient (int fd) {
		m_observers_index	.emplace	(fd, m_observers.size ());
		m_observers		.push_back	({fd, POLLIN});
	}

	void Server::RemoveClient (int fd) {
		int client_index 	= m_observers_index.at 	 (fd);
		int last_fd 		= m_observers	   .back ().fd;

		if (client_index != m_observers.size () - 1) {
			m_observers[client_index] = m_observers.back ();
			m_observers_index[last_fd] = client_index;
		}
		m_observers.pop_back ();
		m_observers_index.erase (fd);
		
	}

	bool Server::HeaderHandler (int & client_index, int bytes, int client_fd) {
		if (bytes < 0) {
			Log ("{C:RED}Error when reading header");
			Log ("{C:RED}Discarding...");
			
			++client_index;

			return (false);
		}
		else if (bytes == 0) {
			Log ("{C:RED}EOF reached");
			Log ("{C:RED}Disconnecting Client #1#...", client_fd);
			
			close (client_fd);
			m_handler->OnDisconnect (*this, client_fd);
			RemoveClient (client_fd);

			return (false);
		}
		else if (bytes != HEADER_SIZE) {
			Log ("{C:RED}Client sent wrong number of bytes (header)");
			Log ("{C:RED}Discarding...");

			++client_index;
			
			return (false);
		}

		return (true);
	}

	bool Server::BodyHandler (int & client_index, int bytes, int body_size, int client_fd, char * body_buffer) {
		if (bytes < 0) {
			Log ("{C:RED}#1# bytes read", bytes);
			Log ("{C:RED}Error when reading body");
			Log ("{C:RED}> #1#", std::strerror (errno));
			
			close (client_fd);
			m_handler->OnDisconnect (*this, client_fd);
			RemoveClient (client_fd);
			
			free (body_buffer);
			
			return (false);
		}
		else if (bytes == 0) {
			Log ("{C:RED}EOF reached");
			Log ("{C:RED}Disconnecting Client #1#...", client_fd);
			
			close (client_fd);
			m_handler->OnDisconnect (*this, client_fd);
			RemoveClient (client_fd);

			free (body_buffer);
			
			return (false);
		}
		else if (bytes != body_size) {
			Log ("{C:RED}Client sent wrong number of bytes (body)");
			Log ("{C:RED}Read bytes: #1#; Expected bytes: #2#", bytes, body_size);

			close (client_fd);
			m_handler->OnDisconnect (*this, client_fd);
			RemoveClient (client_fd);

			free (body_buffer);
			
			return (false);
		}
		
		return (true);
	}

}