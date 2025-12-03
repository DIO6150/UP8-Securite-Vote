#pragma once

#include <utils.hpp>
#include <server_hooks.hpp>
#include <server.hpp>
#include <command_handler.hpp>
#include <rsa.hpp>

#include <algorithm>
#include <ctime>
#include <sstream>

#include <gmp.h>

enum class ConnectionState : int {
	NO_AUTH,
	PUB_KEY_KNOWN,
	PUB_KEY_CONFIRMED,
	FULL_AUTH
};

struct Client {
	int 		socket;
	bool		is_admin;

	time_t		last_activity;
	mpz_class	pub_key;

	ConnectionState	status;

	int		pub_key_proof;

	Client (int socket) : 
		socket {socket}, 
		is_admin {false}, 
		last_activity {-1}, 
		pub_key {0},
		status {ConnectionState::NO_AUTH} {

	}

	Client () : Client (0) {

	}

	bool IsAuth () {
		return (status == ConnectionState::FULL_AUTH);
	}

	bool IsAdmin () {
		return (is_admin && IsAuth ());
	}

	bool IsMidAuth () {
		return (status == ConnectionState::PUB_KEY_CONFIRMED);
	}
};
