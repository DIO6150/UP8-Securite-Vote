
#include "headers/server.hpp"
#include "headers/utils.hpp"

#include <algorithm>
#include <ctime>
#include <sstream>


struct Properties {
	std::string user_name;
	
	Properties () : user_name {} {
		
	}
};


// TODO : replace all perror with fprintf

int main (int argc, char **argv)
{
	char* port = nullptr;
	const char* default_port = "12345";
	
	if (argc == 2) port = argv [1];
	else port = (char *) default_port;
	
	Server<Properties> server {port};
	#define COMMANDS_PARAM std::vector<std::string> args, Client<Properties> & client, Server<Properties> * server
	
	server.SetConnectionCommand ([] (COMMANDS_PARAM) {
		server->Log ("Client {C:GOLD}#1#{} joined", client.GetID ());
		server->Send (client, "Salut");
	});

	server.SetDisconnectionCommand ([] (COMMANDS_PARAM) {
		server->Log ("Client {C:GOLD}#1#{} disconnected", client.GetID ());
		server->Send (client, "Au revoir");
	});

	server.SetEchoCommand ([] (COMMANDS_PARAM) {
		if (client.GetCommandStatus () == CommandStatus::VALID) server->Log ("Client {C:GOLD}#1#{} issued the command: {C:GREEN}#2#{}", client.GetID (), args[0]);
	});

	server.SetUnknownCommand ([] (COMMANDS_PARAM) {
		server->Log ("{C:RED}Error: Command {C:GREEN}#1#{C:RED} issued by Client {C:GOLD}#2#{C:RED} does not exist.", args[0], client.GetID ());
	});

	server.SetCommand ("STOP", [](COMMANDS_PARAM) {
		server->Broadcast ("Server shutting down\n");
		server->Stop ();
	});

	server.Start ();
}