#include "stdafx.h"
#include "NavMesh.h"
#include "../ECS/Entity.h"
#include "AssetLoader.h"
#include "../Renderer/Transform.h"
#include "../ECS/Scene.h"

NavMesh::NavMesh()
{
	m_NumScenes = 0;
}

NavMesh::~NavMesh()
{
	if (m_NumScenes == 0)
	{
		for (NavQuad* quad : m_NavQuads)
		{
			delete quad->entity;
		}
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
	std::string name = "NavQuad" + std::to_string(m_NavQuads.size());
	temp->entity = new Entity(name);

	Model* model = AssetLoader::Get()->LoadModel(L"../Vendor/Resources/Models/NavQuad/navQuadAlt.obj");

	// components
	component::ModelComponent* mc = temp->entity->AddComponent<component::ModelComponent>();
	component::TransformComponent* tc = temp->entity->AddComponent<component::TransformComponent>();
	component::BoundingBoxComponent* bbc = temp->entity->AddComponent<component::BoundingBoxComponent>();

	mc->SetModel(model);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPACITY | FLAG_DRAW::NO_DEPTH);
	Transform* t = tc->GetTransform();
	t->SetScale(size.x, 1.0f, size.y);
	t->SetPosition(position.x, position.y, position.z);
	t->SetRotationY(PI / 2);

	bbc->Init();

	m_NavQuads.push_back(temp);

	return temp;
}

void NavMesh::ConnectNavQuadsByQuad(NavQuad* nav1, NavQuad* nav2, float3 position)
{
	Connection* temp = new Connection();
	temp->position = position;
	temp->quadOne = nav1;
	nav1->connections.push_back(temp);
	temp->quadTwo = nav2;
	nav2->connections.push_back(temp);
	m_Connections.push_back(temp);
}

void NavMesh::ConnectNavQuadsById(int nav1, int nav2, float3 position)
{
	Connection* temp = new Connection();
	temp->position = position;
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

void NavMesh::AddToScene(Scene* scene)
{
	for (NavQuad* quad : m_NavQuads)
	{
		scene->AddEntityFromOther(quad->entity);
	}
	++m_NumScenes;
}
