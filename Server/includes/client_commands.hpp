#pragma once

#include <algorithm>
#include <iterator>

#include <server.hpp>
#include <client.hpp>
#include <context.hpp>

// TODO change f prototype: name, args, ctx

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

static inline void vote_end (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.client.IsAdmin ()) {
		context.server.Send (context.client.socket, "ERROR CODE VOTE E6");
		return;
	}

	if (!context.vote.started) {
		context.server.Send (context.client.socket, "ERROR CODE VOTE E13");
		return;
	}

	context.vote.started = false;

	std::vector<mpz_class> sum;
	sum.reserve (context.vote.candidates.size ());

	for (auto & [id, client] : context.clients) {
		for (int i = 0; i < context.client.vote.size (); ++i) {
			client.aproved = client.aproved && paillier::zkp_verify (context.client.vote[i], context.client.proofs[i], context.vote.pallier);
		}
	}

	for (int i = 0; i < sum.size (); ++i) {
		for (auto & [id, client] : context.clients) {
			if (client.voted && client.aproved) {
				paillier::add (sum[i], client.vote[i], context.vote.pallier);
			}
		}
	}

	std::string results = "";

	for (auto & s : sum) {
		results = StrArgs ("#1# #2#", results, s.get_str ());
	}

	results = StrArgs ("SEND_VOTE_RESULT #1#", results);

	context.server.Broadcast (results);
}

static inline void client_stop(const std::vector<std::string> & args, CommandContext & context) {
	if (context.client.IsAdmin ()) {
		context.server.Stop ();
	}
}

static inline void client_vote (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.client.IsAuth ()) {
		context.server.Send (context.client.socket, "RETURN CODE VOTE E1");
		return;
	}
	
	size_t candidate_count = args.size ();
	
	if (candidate_count != context.vote.candidates.size ()) {
		context.server.Send (context.client.socket, "RETURN CODE VOTE E4");
	}

	context.client.voted = true;
	context.client.vote.resize (args.size ());
	
	for (int index = 0; index < context.client.vote.size (); ++index) {
		context.client.vote[index].set_str (args[index], 10);
	}

	context.server.Send (context.client.socket, "RETURN CODE VOTE O1");
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
	Server::Log ("[Client {C:GOLD}#1#{}] Authenticated.", context.client.socket);
}

static inline void a_client_vote_begin (const std::vector<std::string> & args, CommandContext & context) {
	context.vote.candidates = args;
	context.vote.started = true;

	for (const auto & c : context.vote.candidates) {
		Server::Log ("#1#", c);
	}
}

static inline void client_send_key (const std::vector<std::string> & args, CommandContext & context) {
	if (context.client.status != ConnectionState::NO_AUTH) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY E12");
		return;
	}

	if (args.size () != 1) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY E4");
		return;
	}

	std::string key = args[0];

	if (key.size () < (2048 / 8)) {
		Server::Log ("{C:RED}Received invalid pub_key: {C:GOLD}#1#{} from client {C:GREEN}#2#{}. Wrong Length.", context.client.pub_key.get_str (), context.client.socket);
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY E7");
		return;
	}

	if (context.client.pub_key.set_str (key.substr (0, (2048 / 8)), 10) < 0) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY E7");
		return;
	}
	
	// client.pub_key = rsa_decrypt (client.pub_key, prv_key);
	context.client.status = ConnectionState::PUB_KEY_KNOWN;

	Server::Log ("Received pub_key {C:GOLD}#1#{} from client {C:GREEN}#2#{}.", context.client.pub_key.get_str (), context.client.socket);
	// TODO : select n from 0 to N pseudo randomly doesnt need to be that complicated
	context.client.pub_key_proof = 0;
	
	std::string proof_str = std::to_string (context.client.pub_key_proof); //StrArgs ("#1#", rsa::code (client.pub_key_proof, &client.pub_key ).get_str ());
	context.server.Send (context.client.socket, StrArgs ("SEND_KEY_PROOF #1#", proof_str));
	Server::Log ("Waiting for key confirmation for client {C:GREEN}#1#{}.", context.client.socket);
}

static inline void client_send_key_proof (const std::vector<std::string> & args, CommandContext & context) {
	if (context.client.status != ConnectionState::PUB_KEY_KNOWN) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PROOF E12");
		return;
	}

	if (args.size () != 1) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PROOF E4");
		return;
	}

	int n_p;
	// mpz_class t;
	// t = rsa::code (client.pub_key, key_pair[1]);
	//n_p = t.get_si ();

	n_p = std::stoi (args[0]);
		
	if (n_p != context.client.pub_key_proof + 1) {
		Server::Log ("{C:BLUE}(DEBUG) Expected proof: #1# ; Proof gotten : #2#", context.client.pub_key_proof + 1, n_p);
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PROOF E8");
		context.client.status = ConnectionState::NO_AUTH;
		return;
	}

	context.client.status = ConnectionState::PUB_KEY_CONFIRMED;

	context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PROOF O2");
	Server::Log ("{C:BLUE}(DEBUG) Key for Client [#1#] validated.", context.client.socket);
}

static inline void client_get_candidates (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.vote.started) {
		context.server.Send (context.client.socket, "ERROR CODE GET_CANDIDATES E13");
		return;
	}

	std::string candidates = "";

	for (const auto & candidat : context.vote.candidates) {
		candidates = StrArgs ("#1#/#2#", candidates, candidat);
	}

	candidates = StrArgs ("RETURN CHAR GET_CANDIDATES #1#", candidates);

	context.server.Send (context.client.socket, candidates);
}

static inline void client_send_pallier_key (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.client.IsAdmin ()) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PALLIER E6");
		return;
	}

	if (args.size () != 2) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PALLIER E4");
		return;
	}

	if (args[0].size () != 2048 / 8 || args[1].size () != 2048 / 8) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_KEY_PALLIER E7");
		return;
	}

	mpz_class n;
	mpz_class g;
	
	n.set_str (args[0], 10);
	g.set_str (args[1], 10);

	context.vote.pallier = paillier::PublicKey {n, g};
	Server::Log ("{C:BLUD}(DEBUG) paillier key received");
}

static inline void client_send_zkp (const std::vector<std::string> & args, CommandContext & context) {
	if (!context.client.IsAdmin ()) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E6");
		return;
	}

	int index;
	int n;

	try {
		index	= std::stoi (args[0]);
		n	= std::stoi (args[1]);
	}
	catch (const std::invalid_argument) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E5");
		return;
	}

	if (index >= context.vote.candidates.size ()) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E5");
		return;
	}

	context.client.proofs.resize (args.size ());

	for (int i = 0; i < n; ++i) {
		if (context.client.proofs[index].a_values[i].set_str (args[2 + 0 * n + i], 10) != 0) {
			context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E5");
			return;
		}

		if (context.client.proofs[index].z_values[i].set_str (args[2 + 1 * n + i], 10) != 0)  {
			context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E5");
			return;
		}

		if (context.client.proofs[index].z_values[i].set_str (args[2 + 2 * n + i], 10) != 0)  {
			context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E5");
			return;
		}
	}

	if (context.client.proofs[index].e.set_str (args[2 + 3 * n], 10) != 0) {
		context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP E5");
		return;
	}

	context.server.Send (context.client.socket, "RETURN CODE SEND_ZKP O4");

}

