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

void Network::SendPositionPacket()
{
    sf::Packet packet;

    float3 pos = m_Players.at(0)->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
    float3 mov;
    mov.x = m_Players.at(0)->GetComponent<component::TransformComponent>()->GetTransform()->GetMovement().x;
    mov.y = m_Players.at(0)->GetComponent<component::TransformComponent>()->GetTransform()->GetMovement().y;
    mov.z = m_Players.at(0)->GetComponent<component::TransformComponent>()->GetTransform()->GetMovement().z;

    packet << 1 << pos.x << pos.y << pos.z << mov.x << mov.y << mov.z;

    m_Socket.send(packet);
}

void Network::GetPlayerPosition()
{
    sf::Packet packet;

    if (ListenPacket(&packet))
    {
        int id;
        float3 pos;
        float3 mov;

        packet >> id;

        packet >> pos.x;
        packet >> pos.y;
        packet >> pos.z;

        packet >> mov.x;
        packet >> mov.y;
        packet >> mov.z;


        m_Players.at(id)->GetComponent<component::TransformComponent>()->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
        m_Players.at(id)->GetComponent<component::TransformComponent>()->GetTransform()->SetMovement(mov.x, mov.y, mov.z);
    }
}

void Network::SetPlayerEntityPointer(Entity* playerEnitity, int id)
{
    if (m_Players.size() > id)
    {
        m_Players.at(id) = playerEnitity;
    }
    else 
    {
        m_Players.push_back(playerEnitity);
    }
}

bool Network::ListenPacket(sf::Packet* packet)
{
    m_Socket.setBlocking(false);

    if (m_Socket.receive(*packet) == sf::Socket::Done)
    {
        return true;
    }

    return false;
}
