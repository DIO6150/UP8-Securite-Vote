#pragma once

#include <string>
#include <vector>

struct Vote {
	bool				started;
	std::vector<std::string>	candidates;

	Vote () : started {false} {}
};