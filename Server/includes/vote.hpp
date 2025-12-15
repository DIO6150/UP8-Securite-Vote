#pragma once

#include <string>
#include <vector>
#include <gmp.h>

struct Vote {
	bool				started;
	std::vector<std::string>	candidates;
	
	mpz_class			pallier_pub_key;

	Vote () : started {false} {}
};