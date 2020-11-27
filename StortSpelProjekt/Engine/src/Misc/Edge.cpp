#include "Edge.h"

#include "../ECS/Entity.h"
#include "../Events/EventBus.h"

Edge::Edge(unsigned int id)
{
	m_Id = id;
	m_pConnectedWall = nullptr;
	m_pNavTriangle = nullptr;
}

Edge::~Edge()
{
}

void Edge::AddEntity(Entity* ent)
{
	m_Entities.push_back(ent);
}

void Edge::RemoveEntitiesFromWorld()
{
	for (auto ent : m_Entities)
	{
		EventBus::GetInstance().Publish(&RemoveMe(ent));
	}
	m_Entities.clear();
}

void Edge::ConnectToWall(Edge* wallToConnectTo)
{
	m_pConnectedWall = wallToConnectTo;
	if (!wallToConnectTo->IsConnected())
	{
		wallToConnectTo->ConnectToWall(this);
	}
}

bool Edge::IsConnected()
{
	return m_pConnectedWall != nullptr;
}

Edge* Edge::GetConnectedWall()
{
	return m_pConnectedWall;
}

unsigned int Edge::GetId()
{
	return m_Id;
}

void Edge::AddNavTriangle(NavTriangle* tri)
{
	m_pNavTriangle = tri;
}

NavTriangle* Edge::GetNavTriangle()
{
	return m_pNavTriangle;
}

