#ifndef NETWORK_H
#define NETWORK_H

#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "SFML/Network.hpp"
#include "../Events/EventBus.h"

class Network {
public:
	enum E_PACKET_ID {
		SERVER_DATA = 0,
		PLAYER_DATA = 1,
		PLAYER_DISCONNECT = 2
	};
	struct Player {
		Entity* entityPointer;
		int clientId;
	};

	Network();
	~Network();

	bool ConnectToIP(std::string ip, int port);


	bool IsConnected();
	sf::TcpSocket* GetSocket();

	void SendPositionPacket();
	void Disconnect();
	//Give network the entity pointer for player.
	void SetPlayerEntityPointer(Entity* playerEnitity, int id);

	bool ListenPacket();

private:

	void processPacket(sf::Packet *packet);
	void processPlayerData(sf::Packet* packet);
	void processServerData(sf::Packet* packet);
	void processPlayerDisconnect(sf::Packet* packet);

	sf::TcpSocket m_Socket;
	sf::TcpListener m_Listener;

	std::vector<Player*> m_Players;

	bool m_Connected;
	int m_Id;
};


#endif