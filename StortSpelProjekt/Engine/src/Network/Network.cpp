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

std::string Network::ListenPacket()
{
    m_Socket.setBlocking(false);
    m_Socket.receive(m_PacketRecieve);
    std::string str;
    m_PacketRecieve >> str;

    return str;
}
