#include "stdafx.h"
#include "NavMesh.h"
#include "../ECS/Entity.h"
#include "AssetLoader.h"
#include "../Renderer/Transform.h"
#include "../ECS/Scene.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/BoundingBoxPool.h"

NavMesh::NavMesh(Scene* scene, const std::string& type)
{
	m_NumScenes = 0;

	m_pEntity = scene->AddEntity("NavMesh");

	m_Type = type;

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
	while (m_NavTriangles.size() > 0)
	{
		delete m_NavTriangles.at(0);
		m_NavTriangles.erase(m_NavTriangles.begin());
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
	temp->id = m_NavQuads.size();

	m_NavQuads.push_back(temp);

	return temp;
}

NavTriangle* NavMesh::AddNavTriangle(float3 vertex1, float3 vertex2, float3 vertex3)
{
	NavTriangle* temp = new NavTriangle();
	temp->vertex1 = vertex1;
	temp->vertex2 = vertex2;
	temp->vertex3 = vertex3;
	temp->center = { (vertex1.x + vertex2.x + vertex3.x) / 3.0f, 0.0f, (vertex1.z + vertex2.z + vertex3.z) / 3.0f };
	temp->id = m_NavTriangles.size();

	m_NavTriangles.push_back(temp);

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

void NavMesh::ConnectNavTriangles(NavTriangle* nav1, NavTriangle* nav2)
{
	Connection* temp = new Connection();
	temp->triOne = nav1;
	nav1->connections.push_back(temp);
	temp->triTwo = nav2;
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

void NavMesh::ConnectNavTriangles(int nav1, int nav2)
{
	Connection* temp = new Connection();
	temp->triOne = m_NavTriangles.at(nav1);
	temp->triOne->connections.push_back(temp);
	temp->triTwo = m_NavTriangles.at(nav2);
	temp->triTwo->connections.push_back(temp);

	m_Connections.push_back(temp);
}

NavQuad* NavMesh::GetQuad(float3 position)
{
	float minDist = D3D12_FLOAT32_MAX;
	int closestQuad = 0;
	for (NavQuad* quad : m_NavQuads)
	{

		float	left = quad->position.x - (quad->size.x / 2.0),
				right = quad->position.x + (quad->size.x / 2.0),
				back = quad->position.z - (quad->size.y / 2.0),
				forward = quad->position.z + (quad->size.y / 2.0);
		float dist = D3D12_FLOAT32_MAX;
		if (left <= position.x && position.x <= right)
		{
			if (back <= position.z && position.z <= forward)
			{
				return quad;
			}
			dist = std::min(abs(position.z - forward), abs(position.z - back));
		}
		else if (back <= position.z && position.z <= forward)
		{
			if (left <= position.x && position.x <= right)
			{
				return quad;
			}
			dist = std::min(abs(position.x - left), abs(position.x - right));
		}
		else
		{
			dist = float3({ std::min(abs(position.x - left), abs(position.x - right)), 0.0f, std::min(abs(position.z - forward), abs(position.z - back)) }).length();
		}

		if (dist < minDist)
		{
			minDist = dist;
			closestQuad = quad->id;
		}
	}

	return m_NavQuads.at(closestQuad);
}

NavTriangle* NavMesh::GetTriangle(float3 position)
{
	float smallestDiff = 500.0f;
	int closestTriangle = 0;
	for (NavTriangle* tri : m_NavTriangles)
	{
		float A = triangleArea(tri->vertex1, tri->vertex2, tri->vertex3);
		float A1 = triangleArea(position, tri->vertex2, tri->vertex3);
		float A2 = triangleArea(tri->vertex1, position, tri->vertex3);
		float A3 = triangleArea(tri->vertex1, tri->vertex2, position);

		if (std::abs(A - (A1 + A2 + A3)) < EPSILON)
		{
			return tri;
		}
		else if (std::abs(A - (A1 + A2 + A3)) < smallestDiff)
		{
			smallestDiff = std::abs(A - (A1 + A2 + A3));
			closestTriangle = tri->id;
		}
	}

	return m_NavTriangles.at(closestTriangle);
}

std::vector<NavQuad*> NavMesh::GetAllQuads()
{
	return m_NavQuads;
}

std::vector<NavTriangle*> NavMesh::GetAllTriangles()
{
	return m_NavTriangles;
}

int NavMesh::GetNumQuads()
{
	return m_NavQuads.size();
}

int NavMesh::GetNumTriangles()
{
	return m_NavTriangles.size();
}

const std::string& NavMesh::GetType()
{
	return m_Type;
}

void NavMesh::CreateQuadGrid()
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

		std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("NavMesh" + std::to_string(i++));

		m_pEntity->GetComponent<component::BoundingBoxComponent>()->AddBoundingBox(&bbd, t, name);
	}
	
	++m_NumScenes;
}

void NavMesh::CreateTriangleGrid()
{
	std::vector<unsigned int> indicesVector;
	std::vector<Vertex> verticesVector;
	Vertex vertices[3] = {};
	unsigned int i = 0;
	for (NavTriangle* tri : m_NavTriangles)
	{
		vertices[0].pos = DirectX::XMFLOAT3({ tri->vertex1.x, tri->vertex1.y, tri->vertex1.z });
		vertices[1].pos = DirectX::XMFLOAT3({ tri->vertex2.x, tri->vertex2.y, tri->vertex2.z });
		vertices[2].pos = DirectX::XMFLOAT3({ tri->vertex3.x, tri->vertex3.y, tri->vertex3.z });

		verticesVector.clear();
		for (const Vertex& vertex : vertices)
		{
			verticesVector.push_back(vertex);
		}

		unsigned int indices[3] = {};
		indices[0] = 0; indices[1] = 1; indices[2] = 2;

		indicesVector.clear();
		for (unsigned int index : indices)
		{
			indicesVector.push_back(index);
		}

		BoundingBoxData bbd = {};
		bbd.boundingBoxVertices = verticesVector;
		bbd.boundingBoxIndices = indicesVector;

		Transform* t = m_pEntity->GetComponent<component::TransformComponent>()->GetTransform();

		std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("NavMesh" + std::to_string(i++));

		m_pEntity->GetComponent<component::BoundingBoxComponent>()->AddBoundingBox(&bbd, t, name);
	}

	++m_NumScenes;
}

float NavMesh::triangleArea(float3 vertex1, float3 vertex2, float3 vertex3)
{
	return std::abs(vertex1.x * (vertex2.z - vertex3.z) + vertex2.x * (vertex3.z - vertex1.z) + vertex3.x * (vertex1.z - vertex2.z)) / 2.0f;
}
