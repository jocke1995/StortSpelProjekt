#include "stdafx.h"
#include "Network.h"

#include "../ImGUI/ImGuiHandler.h"

Network::Network()
{
    m_Connected = false;

    m_Players.push_back(new Player);
    m_Players.at(0)->clientId = 0;
}

Network::~Network()
{
    for (int i = 0; i < m_Players.size(); i++)
    {
        m_Players.at(i)->entityPointer = nullptr;
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

void Network::Disconnect()
{
    /* Expected packet configuration
    int playerID;
    */
    
    if (m_Connected) //We should only disconnect if we have connected
    {
        sf::Packet packet;

        packet << E_PACKET_ID::PLAYER_DISCONNECT;
        packet << m_Players.at(0)->clientId;

        m_Socket.send(packet);

        m_Connected = false;
        //Default the first position
        m_Players.at(0)->entityPointer = nullptr; 
        m_Players.at(0)->clientId = 0;
        int size = m_Players.size();
        for (int i = 1; i < size; i++)
        {
            delete m_Players.at(i);
            m_Players.erase(m_Players.begin() + i);
        }
        m_Socket.disconnect();
        m_Socket.setBlocking(true);
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

    m_Socket.send(packet);
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

    switch (packetId)
    {
    case E_PACKET_ID::SERVER_DATA: processServerData(packet); break;
    case E_PACKET_ID::PLAYER_DATA: processPlayerData(packet); break;
    case E_PACKET_ID::PLAYER_DISCONNECT: processPlayerDisconnect(packet); break;
    default: Log::PrintSeverity(Log::Severity::CRITICAL, "Unkown packet id recieved with enum " + std::to_string(packetId));
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

void Network::processPlayerDisconnect(sf::Packet* packet)
{
    /* Expected packet configuration
    int playerID;
    */

    int playerId;
    *packet >> playerId;

    int index = -1;

    //Find player with id
    for (int i = 0; i < m_Players.size(); i++)
    {
        if (m_Players.at(i)->clientId == playerId)
        {
            index = i;
        }
    }

    if (index == -1)
    {
        Log::Print("Recieved to disconnect from unkown player id " + std::to_string(playerId));
    }
    else
    {
        m_Players.at(index)->entityPointer = nullptr; //This does not remove entity. Should be remade when dynamic entity removal is implemented
        delete m_Players.at(index);
        m_Players.erase(m_Players.begin() + index);

        Log::Print("Player " + std::to_string(playerId) + " was disconnected");
    }
}
