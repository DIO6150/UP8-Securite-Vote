#pragma once

#include <string>
#include <vector>
#include <gmp.h>
#include <paillier.hpp>

struct Vote {
	bool				started;
	std::vector<std::string>	candidates;
	
	paillier::PublicKey		pallier;

	Vote () : started {false} {}
};