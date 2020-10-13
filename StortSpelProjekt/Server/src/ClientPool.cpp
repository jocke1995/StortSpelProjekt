#include "stdafx.h"
#include "ClientPool.h"
#include "Network/Network.h"

ClientPool::ClientPool(int port)
{
	// bind the listener to a port
	if (m_Listener.listen(port) != sf::Socket::Done)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Failed attempting to listen to port: " + std::to_string(port) + " failed\n");
	}
	m_Selector.add(m_Listener);

	m_pAvailableClient = nullptr;
	m_pHostClient = nullptr;
	m_AvailableClientId = 0;
}

void ClientPool::ListenMessages()
{
	//Listen if a client sent a package or listener recieved a connection
	if (m_Selector.wait(sf::seconds(0.1f)))
	{
		//Connection Listener
		if (m_Selector.isReady(m_Listener))
		{
			//New possible connection
			newConnection();
		}
		else
		{
			for (int i = 0; i < m_Clients.size(); i++)
			{
				if(m_Selector.isReady(m_Clients.at(i)->socket))
				{
					newPacket(i);
				}
			}
		}
	}
}

void ClientPool::Update(double dt)
{
	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients.at(i)->connected)
		{
			//TIMEOUT
			m_Clients.at(i)->lastPacket += dt;
			if (m_Clients.at(i)->lastPacket >= CLIENT_TIMEOUT)
			{
				m_ConsoleString += "Client " + std::to_string(m_Clients.at(i)->clientId) + " has timed out\n";
				disconnect(i);
			}
			//PLAYER POSITION
			sendPlayerPositions();
		}
	}
}

void ClientPool::AddClient()
{
	m_Clients.push_back(new Client);
	if (m_pAvailableClient == nullptr)
	{
		m_pAvailableClient = m_Clients.at(m_Clients.size() - 1);
	}
}

int ClientPool::GetNrOfClients()
{
	return m_Clients.size();
}

int ClientPool::GetNrOfConnectedClients()
{
	int count = 0;
	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients.at(i)->connected == true)
		{
			count++;
		}
	}
	return count;
}

void ClientPool::ToggleShowPackage()
{
	m_ShowPackage = !m_ShowPackage;
}

void ClientPool::SetState(ServerGame* state)
{
	m_State = state;
}

void ClientPool::RemoveUnconnected()
{
	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients.at(i)->connected == false)
		{
			m_Clients.erase(m_Clients.begin() + i);
			i--;
		}
	}
}

std::string ClientPool::GetConsoleString()
{
	std::string temp = m_ConsoleString;
	m_ConsoleString = "";

	return temp;
}

void ClientPool::playerPosition(int index, sf::Packet packet)
{
	float3 position;
	double4 rotation;
	double3 velocity;

	packet >> position.x >> position.y >> position.z;
	packet >> rotation.x >> rotation.y >> rotation.z >> rotation.w;
	packet >> velocity.x >> velocity.y >> velocity.z;

	m_State->UpdateEntity(std::string("player" + std::to_string(m_Clients.at(index)->clientId)), position, rotation, velocity);
}

void ClientPool::enemyData(int index, sf::Packet packet)
{
	/*
	int nrOfEnemies
	for(nrOfEnemies)
		float3 position
		double4 rotation
		double3 movement
		std::string name
		int target
	*/
	int nrOfEnemies;
	packet >> nrOfEnemies;
	for (int i = 0; i < nrOfEnemies; i++)
	{
		EnemyEntity* entity;
		float3 pos;
		double4 rot;
		double3 mov;
		std::string name;
		int target;

		packet >> pos.x >> pos.y >> pos.z;
		packet >> rot.x >> rot.y >> rot.z >> rot.w;
		packet >> mov.x >> mov.y >> mov.z;
		packet >> name;
		packet >> target;
		entity = m_State->GetEnemy(name);
		if (entity == nullptr)
		{
			m_State->AddEnemy(name, pos, rot, mov, target);
		}
	}
}

void ClientPool::sendPlayerPositions()
{
	sf::Packet packet;
	packet << Network::E_PACKET_ID::PLAYER_DATA;
	packet << GetNrOfConnectedClients();

	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients.at(i)->connected)
		{
			ServerEntity* playerEntity;
			playerEntity = m_State->GetEntity("player" + std::to_string(m_Clients.at(i)->clientId));

			packet << m_Clients.at(i)->clientId;
			packet << playerEntity->position.x << playerEntity->position.y << playerEntity->position.z;
			packet << playerEntity->rotation.x << playerEntity->rotation.y << playerEntity->rotation.z << playerEntity->rotation.w;
			packet << playerEntity->velocity.x << playerEntity->velocity.y << playerEntity->velocity.z;
		}
	}
	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients.at(i)->connected)
		{
			sendPacket(i, packet);
		}
	}
}

void ClientPool::disconnect(int index)
{
	m_Selector.remove(m_Clients.at(index)->socket);
	m_Clients.at(index)->connected = false;
	m_Clients.at(index)->socket.disconnect();
	m_Clients.at(index)->lastPacket = 0;

	m_pAvailableClient = m_Clients.at(index);
	m_AvailableClientId = m_Clients.at(index)->clientId;

	m_State->RemoveEntity("player" + std::to_string(index));

	sf::Packet packet;
	packet << Network::E_PACKET_ID::PLAYER_DISCONNECT;
	packet << m_Clients.at(index)->clientId;

	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients.at(i)->connected)
		{
			m_Clients.at(i)->socket.send(packet);
		}
	}

	m_ConsoleString += "Player " + std::to_string(index) + " was disconnected. There are " + std::to_string(GetNrOfConnectedClients()) + " clients connected\n";
}

void ClientPool::newConnection()
{
	//Check if there is an available client slot
	if (m_pAvailableClient != nullptr)
	{
		//Attempt to accept connection
		if (m_Listener.accept(m_pAvailableClient->socket) == sf::Socket::Done)
		{
			m_pAvailableClient->connected = true;
			m_Selector.add(m_pAvailableClient->socket);
			m_pAvailableClient->clientId = m_AvailableClientId;

			if (m_pHostClient == nullptr)
			{
				m_pHostClient = m_pAvailableClient;
				m_ConsoleString = "Assigned client " + std::to_string(m_pHostClient->clientId) + " to be host\n";
			}

			m_State->AddEntity("player" + std::to_string(m_AvailableClientId));

			//Search for an avaible id to give to next client
			for (int i = 0; i < m_Clients.size(); i++)
			{
				bool idFound = false;
				for (int j = 0; j < m_Clients.size(); j++)
				{
					if (i == m_Clients.at(j)->clientId)
					{
						idFound = true;
						break;
					}
				}
				if (!idFound)
				{
					m_AvailableClientId = i;
					break;
				}
			}

			m_ConsoleString = "Client connected to server\n";

			m_pAvailableClient = nullptr;

			//Find a new available client slot
			for (int i = 0; i < m_Clients.size(); i++)
			{
				if (m_Clients.at(i)->connected == false)
				{
					m_pAvailableClient = m_Clients.at(i);
					break;
				}
			}

			//Send a packet of server info to all clients
			/*Packet Layout
			int packetId;
			int connected Player Id;
			int amount of connected players;
			for(amount of connected players)
				int player id;
			int host id;
			*/
			for (int i = 0; i < m_Clients.size(); i++)
			{
				if (m_Clients.at(i)->connected == true)
				{
					sf::Packet packet;
					packet << Network::SERVER_DATA;
					packet << m_Clients.at(i)->clientId;
					packet << GetNrOfConnectedClients();
					for (int j = 0; j < m_Clients.size(); j++)
					{
						if (m_Clients.at(j)->connected == true)
						{
							packet << m_Clients.at(j)->clientId;
						}
					}
					packet << m_pHostClient->clientId;
					sendPacket(i, packet);
				}
			}
		}
	}
}

void ClientPool::newPacket(int socket)
{
	sf::Packet packet;
	if (m_Clients.at(socket)->socket.receive(packet) == sf::Socket::Done)
	{
		int packetId;
		packet >> packetId;

		m_Clients.at(socket)->lastPacket = 0;

		if (m_ShowPackage)
		{
			m_ConsoleString.append("Recieved a packet from with id " + std::to_string(packetId) + " from client " + std::to_string(socket) + "; " + std::to_string(packet.getDataSize()) + " BYTES\n");
		}

		if (DEVELOPERMODE_NETWORKLOG)
		{
			if (m_ClockReceived.StopTimer() > 1.0)
			{
				std::ostringstream oss;
				oss << std::setprecision(8) << m_NrOfBytesReceived;
				std::string str = oss.str();

				m_ConsoleString.append("Total packages received: " + std::to_string(m_NrOfPackagesReceived) + " Size: " + str + " BYTES\n");

				m_ClockReceived.StartTimer();
				m_NrOfBytesReceived = 0;
				m_NrOfPackagesReceived = 0;
			}

			m_NrOfBytesReceived += packet.getDataSize();
			m_NrOfPackagesReceived += 1;
		}	

		switch(packetId) {
		case Network::E_PACKET_ID::PLAYER_DATA:
			playerPosition(socket, packet);
			break;
		case Network::E_PACKET_ID::PLAYER_DISCONNECT:
			disconnect(socket);
			break;
		case Network::ENEMY_DATA:
			enemyData(socket, packet);
			break;
		default: 
			for (int i = 0; i < m_Clients.size(); i++)
			{
				if (i != socket)
				{
					sendPacket(i, packet);
					
				}
			}
			break;
		}
	}
}

void ClientPool::sendPacket(int index, sf::Packet packet)
{
	m_Clients.at(index)->socket.send(packet);
	if (DEVELOPERMODE_NETWORKLOG)
	{
		m_NrOfBytesSent += packet.getDataSize();
		m_NrOfPackagesSent += 1;

		if (m_ClockSent.StopTimer() > 1.0)
		{
			std::ostringstream oss;
			oss << std::setprecision(8) << m_NrOfBytesSent;
			std::string str = oss.str();

			m_ConsoleString.append("Total packages sent: " + std::to_string(m_NrOfPackagesSent) + " Size: " + str + " BYTES\n");

			m_ClockSent.StartTimer();
			m_NrOfBytesSent = 0;
			m_NrOfPackagesSent = 0;
		}
	}
}
