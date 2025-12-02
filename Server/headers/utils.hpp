#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

#include <string_pretty.hpp>


/*
	Thanks to https://stackoverflow.com/a/217605 for this trim function
	trim from start (in place)
*/
inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

/*
	Thanks to https://stackoverflow.com/a/217605 for this trim function
	trim from end (in place)
*/
inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}


/*
	Thanks to https://stackoverflow.com/a/217605 for this trim function
	trim from both ends (in place)
*/
inline void trim(std::string &s) {
	rtrim(s);
	ltrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

/*
	Thanks to https://stackoverflow.com/a/217605 for this trim function
	trim from end (copying)
*/
inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

/*
	Thanks to https://stackoverflow.com/a/217605 for this trim function
	trim from both ends (copying)
*/
inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}


inline bool is_string_valid (std::string s) {
	if (s.length () < 1 || s.length () > 16) return (false);

	const std::string valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";

	for (auto& c : s) {
		if (std::find (valid_chars.begin (), valid_chars.end (), c) == valid_chars.end ()) return (false);
	}

	return (true);
}


/*
	Thanks to https://stackoverflow.com/a/14266139 for the split function
*/
inline std::vector<std::string> split (std::string s, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t pos = 0;
	std::string token;

	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = trim_copy (s.substr(0, pos));
		if (token != "") tokens.push_back(token);
		s.erase(0, pos + delimiter.length());
	}

	std::string t = trim_copy (s);
	if (t != "") tokens.push_back (t);

	return (tokens);
}

template<class ... Args>
void Log (const std::string & message, Args ... args) {
	std::cerr << "[SERVER] " << StrPretty (message, args ...) << "\n";
}

inline void setup_server (const char * port, int & server) {
	struct addrinfo 		hints, *gai, *ai;
	int				err;
	int				yes;

	yes = 1;
	
	bzero (&hints, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((err = getaddrinfo (NULL, port, &hints, &gai)) < 0) {
		Log ("{C:RED}ERROR : getaddrinfo {C:MAGENTA}#1#{}", gai_strerror (err));
		exit (EXIT_FAILURE);
	}

	for (ai = gai; ai != NULL; ai = ai->ai_next) {
		if ((server = socket (ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
			Log ("{C:RED}ERROR : socket{}");
			continue;
		}
		
		if (setsockopt (server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) < 0) {
			Log ("{C:RED}ERROR : setsockopt{}");
			exit (EXIT_FAILURE);
		}

		if (bind (server, ai->ai_addr, ai->ai_addrlen) < 0) {
			close (server);
			Log ("{C:RED}ERROR : bind{}");
			continue;
		}

		break;
	}

	freeaddrinfo (gai);

	if (ai == NULL) {
		Log ("Failed to bind server");
		exit (EXIT_FAILURE);
	}
}