#include "Edge.h"

#include "../ECS/Entity.h"
#include "../Events/EventBus.h"
#include "../Misc/NavMesh.h"

Edge::Edge(unsigned int id)
{
	m_Id = id;
	m_pConnectedWall = nullptr;
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

void Edge::ConnectToWall(Edge* wallToConnectTo, NavMesh* navMesh)
{
	m_pConnectedWall = wallToConnectTo;
	if (!wallToConnectTo->IsConnected())
	{
		int leastTriangles = std::min<int>(m_NavTriangles.size(), wallToConnectTo->GetNumTriangles());
		for (int i = 0; i < leastTriangles; ++i)
		{
			navMesh->ConnectNavTriangles(m_NavTriangles[i], wallToConnectTo->GetNavTriangle(i));
		}
		wallToConnectTo->ConnectToWall(this, navMesh);
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
	m_NavTriangles.push_back(tri);
}

NavTriangle* Edge::GetNavTriangle(unsigned int id)
{
	return m_NavTriangles[id];
}

int Edge::GetNumTriangles()
{
	return m_NavTriangles.size();
}

