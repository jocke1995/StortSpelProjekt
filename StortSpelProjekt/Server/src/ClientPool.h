#ifndef CLIENTPOOL_H
#define CLIENTPOOL_H

#include "SFML/Network.hpp"
#include "Misc/Timer.h"
#include "ServerGame.h"
#include <iomanip>
#include <vector>

class Network;

struct Client
{
	sf::TcpSocket socket;
	int clientId;
	bool connected = false;
	double lastPacket = 0;
};

#define CLIENT_TIMEOUT 10

class ClientPool
{
public:
	ClientPool(int port);

	//Listen for a packet or new connection. *Is non blocking
	void ListenMessages();
	void Update(double dt);

	void AddClient();
	int GetNrOfClients();
	int GetNrOfConnectedClients();

	void ToggleShowPackage();

	void SetState(ServerGame* state);

	//Removes all open clients that have not connected or disconneted
	void RemoveUnconnected();

	//Get messages to write to console
	std::string GetConsoleString();

private:
	std::vector<Client*> m_Clients;
	sf::TcpListener m_Listener;
	sf::SocketSelector m_Selector;

	Client* m_pAvailableClient;
	int m_AvailableClientId;

	Client* m_pHostClient;

	std::string m_ConsoleString;
	
	bool m_ShowPackage = false;

	ServerGame* m_State;

	float m_NrOfBytesSent = 0.0;
	int m_NrOfPackagesSent = 0;

	float m_NrOfBytesReceived = 0.0;
	int m_NrOfPackagesReceived = 0;

	Timer m_ClockSent;
	Timer m_ClockReceived;

	void playerPosition(int index, sf::Packet packet);
	void sendPlayerPositions();
	void disconnect(int id);
	void newConnection();
	void newPacket(int socket);
	void sendPacket(int index, sf::Packet packet);
};

#endif // !CLIENTPOOL_H
