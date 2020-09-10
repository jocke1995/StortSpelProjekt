#include "Engine.h"
#include <iostream>

int main() {

	Network network;

	std::string ip;

	std::cin >> ip;

	if (ip == "1")
	{
		std::cin >> ip;
		std::cout << "Connecting to " + ip << std::endl;
		network.ConnectToIP(ip, 55555);
	}
	else 
	{
		std::cout << "Listening for connections" << std::endl;
		network.ListenConnection(55555);
	}

	std::getchar();

	return 0;
}
