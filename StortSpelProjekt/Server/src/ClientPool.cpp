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
			m_Clients.at(i)->lastPacket += dt;
			if (m_Clients.at(i)->lastPacket >= CLIENT_TIMEOUT)
			{
				m_ConsoleString += "Client " + std::to_string(m_Clients.at(i)->clientId) + " has timed out\n";
				disconnect(i);
			}
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

void ClientPool::toggleShowPackage()
{
	m_ShowPackage = !m_ShowPackage;
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

void ClientPool::disconnect(int index)
{
	m_Selector.remove(m_Clients.at(index)->socket);
	m_Clients.at(index)->connected = false;
	m_Clients.at(index)->socket.disconnect();
	m_Clients.at(index)->lastPacket = 0;

	m_pAvailableClient = m_Clients.at(index);
	m_AvailableClientId = m_Clients.at(index)->clientId;

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
		if (m_ShowPackage)
		{
			m_ConsoleString.append("Recieved a packet from client " + std::to_string(socket) + "; " + std::to_string(packet.getDataSize()) + " BYTES\n");
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

		for (int i = 0; i < m_Clients.size(); i++)
		{
		case Network::E_PACKET_ID::PLAYER_DISCONNECT:
		{
			disconnect(socket);
			break;
		}
		default: 
		{
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
