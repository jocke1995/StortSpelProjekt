#include "stdafx.h"
#include "Network.h"

#include "../ImGUI/ImGuiHandler.h"

Network::Network()
{
    m_Connected = false;

    m_Players.push_back(new Player);
    m_Players.at(0)->clientId = 0;

    m_ClockSent.StartTimer();

}

Network::~Network()
{
    for (int i = 0; i < m_Players.size(); i++)
    {
        delete m_Players.at(i);
    }
}

bool Network::ConnectToIP(std::string ip, int port)
{
    sf::Socket::Status status = m_Socket.connect(ip, port);

    if (status != 0) 
    {
        ImGuiHandler::GetInstance().AddLog("Connection to %s failed", ip.c_str());
        Log::PrintSeverity(Log::Severity::WARNING, "Connection to " + ip + " failed\n");
        return false;
    }
    else {
        ImGuiHandler::GetInstance().AddLog("Connected to %s succesfully", ip.c_str());
        Log::Print("Connected to " + ip + "\n");
        m_Connected = true;

        //Await server respone for info
        m_Socket.setBlocking(true);
        //Expecting server info
        if (!ListenPacket())
        {
            Log::PrintSeverity(Log::Severity::CRITICAL, "Server info was not recieved!\n");
        }
        //Set socket to non-blocking for future listen
        m_Socket.setBlocking(false);

        return true;
    }
}

bool Network::IsConnected()
{
    return m_Connected;
}

sf::TcpSocket* Network::GetSocket()
{
    return &m_Socket;
}

void Network::SendPositionPacket()
{
    sf::Packet packet;

    float3 pos = m_Players.at(0)->entityPointer->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
    double3 mov = m_Players.at(0)->entityPointer->GetComponent<component::CollisionComponent>()->GetLinearVelocity();
    

    packet << E_PACKET_ID::PLAYER_DATA << m_Id << pos.x << pos.y << pos.z << mov.x << mov.y << mov.z;

    sendPacket(packet);
}

void Network::SetPlayerEntityPointer(Entity* playerEnitity, int id)
{
    bool found = false;
    for (int i = 0; i < m_Players.size(); i++) {
        if (m_Players.at(i)->clientId == id)
        {
            found = true;
            m_Players.at(i)->entityPointer = playerEnitity;
            break;
        }
    }
    if (found == false)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Attempted to add entity pointer to non-existing player id " + std::to_string(id));
    }
}

bool Network::ListenPacket()
{
    sf::Packet packet;
    if (m_Socket.receive(packet) == sf::Socket::Done)
    {
        processPacket(&packet);
        return true;
    }

    return false;
}

void Network::processPacket(sf::Packet* packet)
{
    int packetId;
    *packet >> packetId;

    if (DEVELOPERMODE_NETWORKLOG)
    {
        m_NrOfBytesReceived += packet->getDataSize();
        m_NrOfPackagesReceived += 1;

        if (m_ClockReceived.StopTimer() > 1.0)
        {
            ImGuiHandler::GetInstance().AddLog("Total packages received: %d , Size: %f BYTES", m_NrOfPackagesReceived, m_NrOfBytesReceived);

            m_ClockReceived.StartTimer();
            m_NrOfBytesReceived = 0;
            m_NrOfPackagesReceived = 0;
        }
    }

    switch (packetId)
    {
        case E_PACKET_ID::SERVER_DATA: 
            processServerData(packet); 
            break;
        case E_PACKET_ID::PLAYER_DATA: 
            processPlayerData(packet); 
            break;
        default: 
            Log::PrintSeverity(Log::Severity::CRITICAL, "Unkown packet id recieved with enum " + std::to_string(packetId));
    }
}

void Network::processPlayerData(sf::Packet* packet)
{
    /* Expected packet configuration
    int client id
    float3 player position
    double3 player movment(velocity and direction)
    */

    int id;
    float3 pos;
    double3 mov;

    *packet >> id;

    *packet >> pos.x;
    *packet >> pos.y;
    *packet >> pos.z;

    *packet >> mov.x;
    *packet >> mov.y;
    *packet >> mov.z;

    for (int i = 0; i < m_Players.size(); i++)
    {
        if (m_Players.at(i)->clientId == id)
        {
            m_Players.at(i)->entityPointer->GetComponent<component::CollisionComponent>()->SetPosition(pos.x, pos.y, pos.z);
            m_Players.at(i)->entityPointer->GetComponent<component::CollisionComponent>()->SetVelVector(mov.x, mov.y, mov.z);
        }
    }
}

void Network::processServerData(sf::Packet* packet)
{
    /* Expected packet configuration
    int ClientID
    int amount of players
    for amount of players
        int player id
    */
    *packet >> m_Id;
    m_Players.at(0)->clientId = m_Id;
    int playerCount;
    *packet >> playerCount;

    for (int i = 0; i < playerCount; i++)
    {
        int playerId;
        *packet >> playerId;


        //Check if the player already exist in our list
        bool playerFound = false;
        for (int j = 0; j < m_Players.size(); j++)
        {
            if (m_Players.at(j)->clientId == playerId)
            {
                playerFound = true;
                break;
            }
        }
        //If player is not found then create a new one
        if (playerFound == false)
        {
            m_Players.push_back(new Player);
            m_Players.at(m_Players.size() - 1)->clientId = playerId;
            EventBus::GetInstance().Publish(&PlayerConnection(playerId));
        }
    }
}

// Function to send packages from socket
void Network::sendPacket(sf::Packet packet)
{
    m_Socket.send(packet);
    if (DEVELOPERMODE_NETWORKLOG)
    {
        m_NrOfBytesSent += packet.getDataSize();
        m_NrOfPackagesSent += 1;

        if (m_ClockSent.StopTimer() > 1.0)
        {
            ImGuiHandler::GetInstance().AddLog("Total packages sent: %d , Size: %f BYTES", m_NrOfPackagesSent, m_NrOfBytesSent);

            m_ClockSent.StartTimer();
            m_NrOfBytesSent = 0;
            m_NrOfPackagesSent = 0;
        }
    }
}
