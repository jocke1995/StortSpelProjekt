#ifndef CLIENTPOOL_H
#define CLIENTPOOL_H

#include "SFML/Network.hpp"
#include <vector>
#include "Network/Network.h"

struct Client 
{
	sf::TcpSocket socket;
	int clientId;
	bool connected = false;
	int frameCount = 0;
};

class ClientPool
{
public:
	ClientPool(int port);

	//Listen for a packet or new connection. *Is non blocking
	void ListenMessages();

	void AddClient();
	int GetNrOfClients();
	int GetNrOfConnectedClients();

	//Removes all open clients that have not connected or disconneted
	void RemoveUnconnected();

	//Get messages to write to console
	std::string GetConsoleString();

private:
	std::vector<Client*> m_Clients;
	sf::TcpListener m_Listener;
	sf::SocketSelector m_Selector;

	Client* m_AvailableClient;
	int m_AvailableClientId;

	std::string m_ConsoleString;

	void newConnection();
	void newPacket(int socket);
};

#endif // !CLIENTPOOL_H
