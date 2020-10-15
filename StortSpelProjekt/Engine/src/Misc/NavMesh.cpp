#include "stdafx.h"
#include "NavMesh.h"
#include "../ECS/Entity.h"
#include "AssetLoader.h"
#include "../Renderer/Transform.h"
#include "../ECS/Scene.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/BoundingBoxPool.h"

NavMesh::NavMesh(Scene* scene)
{
	m_NumScenes = 0;

	m_pEntity = scene->AddEntity("NavQuad");

	// components
	component::TransformComponent* tc = m_pEntity->AddComponent<component::TransformComponent>();
	component::BoundingBoxComponent* bbc = m_pEntity->AddComponent<component::BoundingBoxComponent>();
}

NavMesh::~NavMesh()
{
	if (m_NumScenes == 0)
	{
		delete m_pEntity;
	}
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

void NavMesh::ConnectNavQuads(NavQuad* nav1, NavQuad* nav2)
{
	Connection* temp = new Connection();
	temp->quadOne = nav1;
	nav1->connections.push_back(temp);
	temp->quadTwo = nav2;
	nav2->connections.push_back(temp);

	m_Connections.push_back(temp);
}

void NavMesh::ConnectNavQuads(int nav1, int nav2)
{
	Connection* temp = new Connection();
	temp->quadOne = m_NavQuads.at(nav1);
	temp->quadOne->connections.push_back(temp);
	temp->quadTwo = m_NavQuads.at(nav2);
	temp->quadTwo->connections.push_back(temp);

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

void NavMesh::CreateGrid()
{
	std::vector<unsigned int> indicesVector;
	std::vector<Vertex> verticesVector;
	Vertex vertices[4] = {};
	unsigned int i = 0;
	for (NavQuad* quad : m_NavQuads)
	{
		vertices[0].pos = DirectX::XMFLOAT3(quad->position.x + (quad->size.x / 2.0f), 0.0f, quad->position.z - (quad->size.y / 2.0f));
		vertices[1].pos = DirectX::XMFLOAT3(quad->position.x - (quad->size.x / 2.0f), 0.0f, quad->position.z - (quad->size.y / 2.0f));
		vertices[2].pos = DirectX::XMFLOAT3(quad->position.x + (quad->size.x / 2.0f), 0.0f, quad->position.z + (quad->size.y / 2.0f));
		vertices[3].pos = DirectX::XMFLOAT3(quad->position.x - (quad->size.x / 2.0f), 0.0f, quad->position.z + (quad->size.y / 2.0f));

		verticesVector.clear();
		for (const Vertex& vertex : vertices)
		{
			verticesVector.push_back(vertex);
		}

		unsigned int indices[6] = {};
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 1; indices[4] = 2; indices[5] = 3;

		indicesVector.clear();
		for (unsigned int index : indices)
		{
			indicesVector.push_back(index);
		}

		BoundingBoxData bbd = {};
		bbd.boundingBoxVertices = verticesVector;
		bbd.boundingBoxIndices = indicesVector;

		Transform* t = m_pEntity->GetComponent<component::TransformComponent>()->GetTransform();

		std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("NavQuad" + std::to_string(i++));

		m_pEntity->GetComponent<component::BoundingBoxComponent>()->AddBoundingBox(&bbd, t, name);
	}
	
	++m_NumScenes;
}
