#include "stdafx.h"
#include "NavMesh.h"

NavMesh::NavMesh()
{
}

NavMesh::~NavMesh()
{
	while (m_NavQuads.size() > 0)
	{
		delete m_NavQuads.at(0);
		m_NavQuads.erase(m_NavQuads.begin());
	}
	while (m_Connections.size() > 0)
	{
		delete m_Connections.at(0);
		m_Connections.erase(m_Connections.begin());
	}
}

NavQuad* NavMesh::AddNavQuad(float3 position, float2 size)
{
	NavQuad* temp = new NavQuad();
	temp->position = position;
	temp->size = size;
	m_NavQuads.push_back(temp);

	return temp;
}

void NavMesh::ConnectNavQuads(NavQuad* nav1, NavQuad* nav2, float3 position)
{
	Connection* temp = new Connection();
	temp->position = position;
	temp->quadOne = nav1;
	nav1->connections.push_back(temp);
	temp->quadTwo = nav2;
	nav2->connections.push_back(temp);
	m_Connections.push_back(temp);
}

NavQuad* NavMesh::GetQuad(float3 position)
{
	for (int i = 0; i < m_NavQuads.size(); i++)
	{
		NavQuad* temp = m_NavQuads.at(i);
		if (temp->position.x - temp->size.x <= position.x && position.x <= temp->position.x + temp->size.x &&
			temp->position.y - temp->size.y <= position.y && position.y <= temp->position.y + temp->size.y &&
			temp->position.z == position.z)
		{
			return temp;
		}
	}

	return nullptr;
}
