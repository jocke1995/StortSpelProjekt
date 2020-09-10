#ifndef NETWORK_H
#define NETWORK_H

#include "../Headers/Core.h"
#include "SFML/Network.hpp"

class Network {
public:
	Network();

	bool ConnectToIP(std::string ip, int port);
	void ListenConnection(int port);

	//Add string to at the end to the next packet
	void AppendPacket(std::string str);
	void SendPacket();

private:
	sf::TcpSocket m_Socket;
	sf::TcpListener m_Listener;
};


#endif