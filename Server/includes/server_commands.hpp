#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <context.hpp>

static inline void server_stop (const std::vector<std::string> & args, ServerCommandContext & context) {
	context.server.Stop ();
}

static inline void server_disconnect (const std::vector<std::string> & args, ServerCommandContext & context) {
	int client_fd;
	for (const auto & a: args) {
		try {
			client_fd = std::stoi (a);
		}
		catch (std::invalid_argument const & ex) {
			Server::Log ("{C:RED}Invalid arg : {C:GOLD}#1#{}", client_fd);
			continue;
		}

		if (!context.server.HasClient (client_fd)) {
			Server::Log ("{C:RED}Client does not exist : {C:GOLD}#1#{}", client_fd);
			continue;
		}

		context.server.Disconnect (client_fd);
	}
}

static inline void server_send (const std::vector<std::string> & args, ServerCommandContext & context) {
	int client_fd;

	try {
		client_fd = std::stoi (args.front ());
	}
	catch (std::invalid_argument const & ex) {
		Server::Log ("{C:RED}Invalid client : {C:GOLD}#1#{}", client_fd);
		return;
	}

	if (!context.server.HasClient (client_fd)) {
		Server::Log ("{C:RED}Client does not exist : {C:GOLD}#1#{}", client_fd);
		return;
	}

	std::string msg;

	for (auto it = args.begin () + 1; it != args.end (); ++it) {
		msg += " " + *it;
	}

	context.server.Send (client_fd, msg);
}

static inline void server_list (const std::vector<std::string> & args, ServerCommandContext & context) {
	Server::Log ("{C:BLUE}List of currently connected clients :");
	for (const auto & c : context.server.ListClients ()) {
		Server::Log ("{C:BLUE}#1#", c);
	}
}