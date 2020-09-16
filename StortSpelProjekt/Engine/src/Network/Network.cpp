#include "stdafx.h"
#include "Network.h"

Network::Network()
{
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
        return true;
    }
}

void Network::ListenConnection(int port)
{
    // bind the listener to a port
    if (m_Listener.listen(port) != sf::Socket::Done)
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Failed attempting to listen to port: " + std::to_string(port) + " failed\n");
    }
    // Accept connection
    if (m_Listener.accept(m_Socket) != sf::Socket::Done)
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Failed connection to " + m_Socket.getRemoteAddress().toString() + "\n");
    }

    Log::Print("Connected to " + m_Socket.getRemoteAddress().toString() + "\n");
}

void Network::AppendStringPacket(std::string str)
{
    m_PacketSend << str;
}

void Network::SendPacket()
{
    m_Socket.send(m_PacketSend);
    m_PacketSend.clear();
}

std::string Network::ListenPacket()
{
    m_Socket.setBlocking(true);
    m_Socket.receive(m_PacketRecieve);
    std::string str;
    m_PacketRecieve >> str;

    return str;
}
