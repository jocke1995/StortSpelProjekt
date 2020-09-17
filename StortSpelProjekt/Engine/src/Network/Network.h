#ifndef NETWORK_H
#define NETWORK_H

#include "../Headers/Core.h"
#include "SFML/Network.hpp"

class Network {
public:
	Network();

	bool ConnectToIP(std::string ip, int port);

	sf::TcpSocket* GetSocket();

	void SendPositionPacket(float3 position);
	float3 GetPlayerPosition(int playerId);

	sf::Packet ListenPacket();

private:
	sf::TcpSocket m_Socket;
	sf::TcpListener m_Listener;

	bool m_Connected;
};


#endif