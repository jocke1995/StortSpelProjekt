#include "stdafx.h"
#include "ServerGame.h"
#include "Network/Network.h"

ServerGame::ServerGame()
{
	m_FrameCount = 0;
}

void ServerGame::StartGameState()
{
	m_FrameCount = 0;
}

void ServerGame::Update(double dt)
{
	m_FrameCount++;
}

void ServerGame::UpdateEntity(std::string name, float3 position, float3 rotation, float3 velocity)
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities.at(i).name == name)
		{
			m_Entities.at(i).position = position;
			m_Entities.at(i).rotation = rotation;
			m_Entities.at(i).velocity = velocity;
			break;
		}
	}
}

ServerEntity* ServerGame::GetEntity(std::string name)
{
	ServerEntity* entity = nullptr;
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities.at(i).name == name)
		{
			entity = &m_Entities.at(i);
			break;
		}
	}
	return entity;
}
