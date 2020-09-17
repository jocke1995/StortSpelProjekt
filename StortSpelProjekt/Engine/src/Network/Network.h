#ifndef NETWORK_H
#define NETWORK_H

#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "SFML/Network.hpp"

class Network {
public:
	Network();

	bool ConnectToIP(std::string ip, int port);

	sf::TcpSocket* GetSocket();

	void SendPositionPacket();
	void GetPlayerPosition();
	
	//Give network the entity pointer for player.
	void SetPlayerEntityPointer(Entity* playerEnitity, int id);

	sf::Packet ListenPacket();

private:
	sf::TcpSocket m_Socket;
	sf::TcpListener m_Listener;

	std::vector<Entity*> m_Players;

	bool m_Connected;
};


#endif