#pragma once

#include <algorithm>
#include <iterator>

#include <server.hpp>
#include <client.hpp>
#include <context.hpp>

static inline void vote_begin (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.client.IsAdmin ()) {
		context.server.Send (context.client.socket, "ERROR CODE VOTE E6");
		return;
	}

	if (context.vote.started) {
		context.server.Send (context.client.socket, "ERROR CODE VOTE E10");
		return;
	}

	context.vote.started = true;
	std::copy (args.begin (), args.end (), back_inserter (context.vote.candidates));
}

static inline void client_stop(const std::vector<std::string> & args, CommandContext & context) {
	if (context.client.IsAdmin ()) {
		context.server.Stop ();
	}
}

static inline void client_vote (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.client.IsAuth ()) {
		context.server.Send (context.client.socket, "ERROR CODE VOTE E1");
		return;
	}

	// TODO : PAILLIER GOES HERE

	size_t candidate_count = args.size ();

	if (candidate_count != context.vote.candidates.size ()) {
		context.server.Send (context.client.socket, "ERROR CODE VOTE E4");
	}
}

static inline void client_login (const std::vector<std::string> & args, CommandContext & context) {
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
}

static inline void a_client_vote_begin (const std::vector<std::string> & args, CommandContext & context) {
	context.vote.candidates = args;

	for (const auto & c : context.vote.candidates) {
		Server::Log ("#1#", c);
	}
}