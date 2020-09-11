#include "Engine.h"
#include <iostream>

int main() {

	Network network;

	std::string ip;


	//This is for testing
	std::cout << "Write 1 to be client or 0 to be server" << std::endl;
	std::cin >> ip;

	if (ip == "1")
	{
		std::cout << "What ip to connect to. (\"localhost\" to connect to own machine)" << std::endl;
		std::cin >> ip;
		std::cout << "Connecting to " + ip << std::endl;
		network.ConnectToIP(ip, 55555);

		std::cout << "Write message" << std::endl;
		std::string str = "";
		std::cin >> str;
		network.AppendStringPacket(str);
		network.SendPacket();
	}
	else
	{
		std::cout << "Listening for connections" << std::endl;
		network.ListenConnection(55555);

		std::string str = "";
		str = network.ListenPacket();

		std::cout << "Message recieved: ";
		std::cout << str;
	}

	std::getchar();
	// TESTING

	return 0;
}
