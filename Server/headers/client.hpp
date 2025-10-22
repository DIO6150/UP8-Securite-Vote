#pragma once

#include <string>
#include <vector>
#include <ctime>

#include <command_status.hpp>


template<class Properties>
class Client {
public:
	Client (int id, int socket) :
		m_id {id},
		m_socket {socket},
		m_logged_in {false},
		m_potentially_dead {false},
		m_is_dead {false} {
			time (&m_last_activity);
		}

	Client () {

	}

	int GetID () const {
		return (m_id);
	}

	int GetSocket () const {
		return (m_socket);
	}

	int IsLoggedIn () const {
		return (m_logged_in);
	}

	void AcceptLogin () {
		m_logged_in = true;
	}

	void SetDeadSuspicionFlag (bool flag) {
		m_potentially_dead = flag;
	}

	bool IsSuspious () const {
		return (m_potentially_dead && !m_is_dead);
	}

	void Kill () {
		m_is_dead = true;
	}

	bool IsDead () const {
		return (m_is_dead);
	}

	void UpdateActivity () {
		time (&m_last_activity);
	}

	time_t GetLastActivity () const {
		return (m_last_activity);
	}

	void UpdateCommandStats (const std::string & command, std::vector<std::string> & args, CommandStatus status) {
		m_last_command = command;
		m_last_command_args = args;
		m_command_status = status;
	}

	CommandStatus GetCommandStatus () const {
		return (m_command_status);
	}

	const std::string & GetLastCommand () const {
		return (m_last_command);
	}

	const std::vector<std::string> & GetLastCommandArgs () const {
		return (m_last_command_args);
	}

	Properties & GetPropeties () {
		return (m_properties);
	}

private:
	int				m_id;
	int				m_socket;

	bool				m_logged_in;	

	bool	   			m_potentially_dead;
	bool	   			m_is_dead;
	time_t				m_last_activity;

	std::string			m_last_command;
	std::vector<std::string>	m_last_command_args;
	CommandStatus			m_command_status;
	
	Properties			m_properties;
};