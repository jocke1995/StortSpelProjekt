#include "stdafx.h"
#include "ClientPool.h"

ClientPool::ClientPool(int port)
{
	// bind the listener to a port
	if (m_Listener.listen(port) != sf::Socket::Done)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Failed attempting to listen to port: " + std::to_string(port) + " failed\n");
	}
	m_Selector.add(m_Listener);

	m_AvailableClient = nullptr;
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
					break;
				}
			}
		}
	}
}

void ClientPool::AddClient()
{
	m_Clients.push_back(new Client);
	if (m_AvailableClient == nullptr)
	{
		m_AvailableClient = m_Clients.at(m_Clients.size() - 1);
	}
}

int ClientPool::GetNrOfClients()
{
	return m_Clients.size();
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
	m_ConsoleString.clear();

	return temp;
}

void ClientPool::newConnection()
{
	//Check if there is an available client slot
	if (m_AvailableClient != nullptr)
	{
		//Attempt to accept connection
		if (m_Listener.accept(m_AvailableClient->socket) == sf::Socket::Done)
		{
			m_AvailableClient->connected = true;
			m_Selector.add(m_AvailableClient->socket);

			m_ConsoleString.append(m_AvailableClient->socket.getRemoteAddress().toString() + " connected to server\n");

			m_AvailableClient = nullptr;

			//Find a new available client slot
			for (int i = 0; i < m_Clients.size(); i++)
			{
				if (m_Clients.at(i)->connected == false)
				{
					m_AvailableClient = m_Clients.at(i);
					break;
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
		m_ConsoleString.append("Recieved a packet from client " + std::to_string(socket) + "; " + std::to_string(packet.getDataSize()) + " BYTES\n");
		for (int i = 0; i < m_Clients.size(); i++)
		{
			if (i != socket)
			{
				if (m_Clients.at(i)->connected)
				{
					m_Clients.at(i)->socket.send(packet);
				}
			}
		}
	}
}
