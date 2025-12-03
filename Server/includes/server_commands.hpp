#pragma once

#include <vector>

#include <context.hpp>

static inline void server_stop (const std::vector<std::string> & args, ServerCommandContext & context) {
	context.server.Stop ();
}