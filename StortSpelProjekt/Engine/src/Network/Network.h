#ifndef NETWORK_H
#define NETWORK_H

#include "../Headers/Core.h"
#include "SFML/Network.hpp"

class Network {
public:
	Network();

	bool ConnectToIP(std::string ip, int port);

	sf::TcpSocket* GetSocket();

	void SendPositionPacket();

	std::string ListenPacket();

private:
	sf::TcpSocket m_Socket;
	sf::TcpListener m_Listener;
	sf::Packet m_PacketRecieve;
	sf::Packet m_PacketSend;

	bool m_Connected;
};


#endif