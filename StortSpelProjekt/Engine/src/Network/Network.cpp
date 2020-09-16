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

sf::TcpSocket* Network::GetSocket()
{
    return &m_Socket;
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
    m_Socket.setBlocking(false);
    m_Socket.receive(m_PacketRecieve);
    std::string str;
    m_PacketRecieve >> str;

    return str;
}
