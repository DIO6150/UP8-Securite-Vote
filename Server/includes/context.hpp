#pragma once

#include <client.hpp>
#include <server.hpp>
#include <vote.hpp>

struct CommandContext {
	Server::Server & server;
	Client & client;
	Vote   & vote;
	std::unordered_map<int, Client> clients;
};

struct ServerCommandContext {
	Server::Server & server;
};