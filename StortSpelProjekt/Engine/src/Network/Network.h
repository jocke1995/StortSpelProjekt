#ifndef NETWORK_H
#define NETWORK_H

#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "SFML/Network.hpp"
#include "../Events/EventBus.h"
#include "../Misc/Timer.h"
#include <iomanip>

class Network {
public:
	enum E_PACKET_ID {
		SERVER_DATA = 0,
		PLAYER_DATA = 1,
		PLAYER_DISCONNECT = 2,
		ENEMY_DATA = 3
	};
	struct Player {
		Entity* entityPointer;
		int clientId;
		bool isHost;
	};

	Network();
	~Network();

	bool ConnectToIP(std::string ip, int port);


	bool IsConnected();
	bool IsHost();

	sf::TcpSocket* GetSocket();

	void SendPositionPacket();
	void SendEnemiesPacket(std::vector<Entity*>* enemies);
	void Disconnect();
	//Give network the entity pointer for player.
	void SetPlayerEntityPointer(Entity* playerEnitity, int id);
	void SetEnemiesEntityPointers(std::vector<Entity*>* enemies);

	bool ListenPacket();

private:

	void processPacket(sf::Packet *packet);
	void processPlayerData(sf::Packet* packet);
	void processServerData(sf::Packet* packet);
	void processEnemyData(sf::Packet* packet);
	void processPlayerDisconnect(sf::Packet* packet);

	void sendPacket(sf::Packet packet);

	sf::TcpSocket m_Socket;
	sf::TcpListener m_Listener;

	std::vector<Player*> m_Players;
	std::vector<Entity*>* m_pEnemies;

	bool m_Connected;
	int m_Id;

	float m_NrOfBytesSent = 0.0;
	int m_NrOfPackagesSent = 0;

	float m_NrOfBytesReceived = 0.0;
	int m_NrOfPackagesReceived = 0;

	Timer m_ClockSent;
	Timer m_ClockReceived;

};


#endif