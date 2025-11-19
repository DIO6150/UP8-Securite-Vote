#include <server_interface.hpp>
#include <utils.hpp>

#include <cstring>

#define HEADER_SIZE 4
#define MAX_BODY_SIZE 65536


Server::Server (IHandler * handler) : Server {"12345", handler} {

}

Server::Server (const char * port, IHandler * handler) :
m_run (true),
m_handler (handler) {
	setup_server (port, m_server);

	Log ("Server created on port: #1#", port);
}

Server::~Server () {
	if (m_server) {
		close (m_server);
	}
}

void Server::Start () {
	int			n_bytes;
	
	sockaddr_storage	from;
	int			length;
	
	char			header_buffer[4];
	char *			body_buffer;

	int32_t			body_size;

	length = sizeof (from);

	if (listen (m_server, 8) < 0) {
		exit (EXIT_FAILURE);
	}

	std::thread listen_thread {[this]() { this->Listen (); }};

	Log ("Server is now running");

	while (m_run) {

		m_connection_queue_mutex.lock ();

		while (!m_connection_queue.empty ()) {
			int client_socket = m_connection_queue.front ();
			m_connection_queue.pop_front ();
			
			m_observers.push_back ({client_socket, POLLIN});
			m_handler->OnConnect (*this, client_socket);
		}

		m_connection_queue_mutex.unlock ();

		while (!m_disconnection_queue.empty ()) {
			int client_socket = m_connection_queue.front ();
			m_disconnection_queue.pop_front ();
			
			auto pos = std::find_if (
				m_observers.begin (),
				m_observers.end (),
				[&client_socket](pollfd poll) {
					return (poll.fd == client_socket);
				}
			);

			close (client_socket);
			m_handler->OnDisconnect (*this, client_socket);
			m_observers.erase (pos);
		}
		

		poll (m_observers.data (), m_observers.size (), 0);


		for (auto it = m_observers.begin (); it != m_observers.end ();) {
			pollfd client_poll = *it;
			int client_fd = client_poll.fd;

			if (client_poll.revents & POLLIN) {
				m_handler->OnUpdate (*this, client_fd);

				bzero (header_buffer, HEADER_SIZE);
				
				if ((n_bytes = read (client_fd, header_buffer, HEADER_SIZE)) < 0) {
					Log ("{C:RED}Error when reading header");
					Log ("{C:RED}> #1#", std::strerror (errno));
					m_handler->OnDisconnect (*this, client_fd);
					close (client_fd);
					it = m_observers.erase (it);
					continue;
				}

				if (n_bytes != 4) {
					m_handler->OnDisconnect (*this, client_fd);
					close (client_fd);
					it = m_observers.erase (it);
					continue;
				}

				body_size = *(int*)header_buffer;
				body_size = ntohl (body_size);
				body_size = body_size >= MAX_BODY_SIZE ? MAX_BODY_SIZE : body_size;

				Log ("{C:GOLD}#1#", body_size);
				
				body_buffer = (char*) malloc (body_size);

				if ((n_bytes = read (client_fd, body_buffer, body_size)) < 0) {
					Log ("{C:RED}Error when reading body");
					Log ("{C:RED}> #1#", std::strerror (errno));
					m_handler->OnDisconnect (*this, client_poll.fd);
					// TODO: do something about the lose close, thechnically in a threaded app, close will do weird things if you are closing a socket at the same time as creating one
					close (client_fd);
					it = m_observers.erase (it);
					continue;
				}

				if (n_bytes != body_size) {
					m_handler->OnDisconnect (*this, client_fd);
					close (client_fd);
					it = m_observers.erase (it);
					continue;
				}
				
				m_handler->OnRequest (*this, client_poll.fd, body_buffer);
				
				free (body_buffer);

			}
			it++;
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

	if (write (client, message.c_str (), message.length ()) <= 0) {
		Log ("{C:RED}Error: couldn't send data to client");
		return;
	}
}

void Server::Disconnect (int client_id) {
	m_disconnection_queue.push_back (client_id);
}

void Server::ForceDisconnect (int client_id) {

}

void Server::Listen () {
	sockaddr_storage	from;
	int			len;
	int			client_socket;

	len = sizeof(from);

	for (;;) {
		if ((client_socket = accept (m_server, (sockaddr *) &from, (socklen_t *) &len)) <= 0) {
			Log ("[LISTENER] Something went wrong: #1#.", client_socket);
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