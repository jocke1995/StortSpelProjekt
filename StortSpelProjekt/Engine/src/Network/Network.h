#ifndef NETWORK_H
#define NETWORK_H

#include "../Headers/Core.h"
#include "SFML/Network.hpp"

class Network {
public:
	Network();

	bool ConnectToIP(std::string ip, int port);

	void ListenConnection(int port);

private:
	sf::TcpSocket socket;
	sf::TcpListener listener;
};


#endif