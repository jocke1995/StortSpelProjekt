#include "stdafx.h"
#include "Network.h"

Network::Network()
{
    m_Connected = false;
}

bool Network::ConnectToIP(std::string ip, int port)
{
    sf::Socket::Status status = m_Socket.connect(ip, port);

    if (status != 0) 
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Connection to " + ip + " failed\n");
        return false;
    }
    else {
        Log::Print("Connected to " + ip + "\n");
        m_Connected = true;
        return true;
    }
}

sf::TcpSocket* Network::GetSocket()
{
    return &m_Socket;
}

void Network::SendPositionPacket(float3 position)
{
    sf::Packet packet;
    packet << position.x << position.y << position.z;

    m_Socket.send(packet);
}

float3 Network::GetPlayerPosition(int playerId)
{
    sf::Packet packet = ListenPacket();

    float3 pos;
    packet >> pos.x;
    packet >> pos.y;
    packet >> pos.z;

    return pos;
}

sf::Packet Network::ListenPacket()
{
    m_Socket.setBlocking(false);

    sf::Packet packet;
    m_Socket.receive(packet);

    return packet;
}
