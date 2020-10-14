#include "stdafx.h"
#include "ServerGame.h"
#include "Network/Network.h"

ServerGame::ServerGame()
{
	m_FrameCount = 0;
}

ServerGame::~ServerGame()
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		delete m_Entities.at(i);
	}
}

void ServerGame::StartGameState()
{
	m_FrameCount = 0;
}

void ServerGame::Update(double dt)
{
	m_FrameCount++;
}

void ServerGame::UpdateEntity(std::string name, float3 position, double4 rotation, double3 velocity)
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities.at(i)->name == name)
		{
			m_Entities.at(i)->position = position;
			m_Entities.at(i)->rotation = rotation;
			m_Entities.at(i)->velocity = velocity;
			break;
		}
	}
}

void ServerGame::UpdateEnemy(std::string name, float3 position)
{
	for (int i = 0; i < m_Enemies.size(); i++)
	{
		if (m_Enemies.at(i)->name == name)
		{
			m_Enemies.at(i)->position = position;
			break;
		}
	}
}

ServerEntity* ServerGame::GetEntity(std::string name)
{
	ServerEntity* entity = nullptr;
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities.at(i)->name == name)
		{
			entity = m_Entities.at(i);
			break;
		}
	}
	return entity;
}

EnemyEntity* ServerGame::GetEnemy(std::string name)
{
	EnemyEntity* entity = nullptr;
	for (int i = 0; i < m_Enemies.size(); i++)
	{
		if (m_Enemies.at(i)->name == name)
		{
			entity = m_Enemies.at(i);
			break;
		}
	}
	return entity;
}

void ServerGame::AddEntity(std::string name)
{
	ServerEntity* temp = new ServerEntity;
	temp->name = name;
	m_Entities.push_back(temp);

}

void ServerGame::AddEnemy(std::string name, float3 position)
{
	EnemyEntity* temp = new EnemyEntity;
	temp->name = name;
	temp->position = position;
	m_Enemies.push_back(temp);
}

void ServerGame::RemoveEntity(std::string name)
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities.at(i)->name == name)
		{
			delete m_Entities.at(i);
			m_Entities.erase(m_Entities.begin() + i);
			break;
		}
	}
}
