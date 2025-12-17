#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <utils.hpp>

namespace Server {
	template<class Context>
	class CommandHandler {
	public:
		using COMMAND_PARAM = std::function<void (const std::vector<std::string> & args, Context & context)>;

		CommandHandler () :
			m_default_command {[](const std::vector<std::string> & args, Context & context) { Log ("command not defined"); }} {

		}

		
		void RegisterCommand (const std::string && name, const COMMAND_PARAM && command) {
			m_commands.emplace (name, command);
		}
		
		void Handle (const std::string name, const std::vector<std::string> & args, Context && context) const {
			const COMMAND_PARAM & command = Get (name);

			command (args, context);
		}

		static std::pair<std::string, std::vector<std::string>> GetTokens (std::string & request) {
			auto tokens = ServerInternal::split (request, " ");

			if (tokens.empty()) return (std::pair<std::string, std::vector<std::string>> {"", {}});

			const auto name = tokens.front ();
			tokens.erase (tokens.begin ());

			return (std::pair<std::string, std::vector<std::string>> {name, tokens});
		}
		
	private:
		const COMMAND_PARAM & Get (const std::string & name) const {
			const auto & pos = m_commands.find (name);

			if (pos == m_commands.end ()) {
				Log ("{C:RED}Command not defined = {C:GREEN X:WHITE}#1#", name);
				return (m_default_command);
			}

			return (pos->second);
		}

		std::unordered_map<std::string, COMMAND_PARAM> m_commands;
		COMMAND_PARAM m_default_command;
	};
}