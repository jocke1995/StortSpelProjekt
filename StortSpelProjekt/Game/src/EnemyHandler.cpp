#include "EnemyHandler.h"
#include "ECS/Scene.h"
#include "Engine.h"

EnemyHandler::EnemyHandler(Scene* scene)
{
	m_pScene = scene;
}

EnemyHandler::~EnemyHandler()
{
	for (auto pair : m_enemyComps)
	{
		if (pair.second != nullptr)
		{
			delete pair.second;
		}
	}
	m_enemyComps.clear();
}

void EnemyHandler::AddEnemy(std::string entityName, Model* model, unsigned int flag, float3 pos, float scale, float3 rot)
{
	for (auto pair : m_enemyComps)
	{
		// An entity with this m_Name already exists
		// so should have used the other overloaded version of AddEnemy
		if (pair.first == entityName)
		{
			AddEnemy(entityName, pos);
			// TODO same but with other options for scale and stuff
			return;
		}
	}
	Entity *ent = m_pScene->AddEntity(entityName);
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	m_enemyComps[entityName] = new EnemyComps;

	m_enemyComps[entityName]->enemiesOfThisType++;
	m_enemyComps[entityName]->compFlags = flag;
	m_enemyComps[entityName]->pos = pos;
	m_enemyComps[entityName]->scale = scale;
	m_enemyComps[entityName]->rot = rot;
	m_enemyComps[entityName]->model = model;

	mc = ent->AddComponent<component::ModelComponent>();
	//m_enemyComps[entityName]->s_Components.push_back("modelComponent");
	tc = ent->AddComponent<component::TransformComponent>();
	//m_enemyComps[entityName]->s_Components.push_back("transformComponent");

	

	mc->SetModel(m_enemyComps[entityName]->model/*model*/);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	tc->GetTransform()->SetScale(scale);
	tc->GetTransform()->SetRotationX(rot.x);
	tc->GetTransform()->SetRotationX(rot.y);
	tc->GetTransform()->SetRotationX(rot.z);
	if (F_COMP_FLAGS::OBB & flag)
	{
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		//m_enemyComps[entityName]->s_Components.push_back("");
		Physics::GetInstance().AddCollisionEntity(ent);
	}

}

void EnemyHandler::AddEnemy(std::string entityName, float3 pos)
{
	for (auto pair : m_enemyComps)
	{
		// An entity with this m_Name already exists
		// so create a new onen of the same type
		if (pair.first == entityName)
		{
			std::string name = entityName + std::to_string(m_enemyComps[entityName]->enemiesOfThisType);
			Entity* ent = m_pScene->AddEntity(name);
			m_enemyComps[entityName]->enemiesOfThisType++;
			component::ModelComponent* mc = nullptr;
			component::TransformComponent* tc = nullptr;
			component::BoundingBoxComponent* bbc = nullptr;
			
			mc = ent->AddComponent<component::ModelComponent>();
			tc = ent->AddComponent<component::TransformComponent>();

			mc->SetModel(m_enemyComps[entityName]->model);
			mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
			tc->GetTransform()->SetPosition(pos.x, pos.y , pos.z);
			tc->GetTransform()->SetScale(m_enemyComps[entityName]->scale);
			tc->GetTransform()->SetRotationX(m_enemyComps[entityName]->rot.x);
			tc->GetTransform()->SetRotationX(m_enemyComps[entityName]->rot.y);
			tc->GetTransform()->SetRotationX(m_enemyComps[entityName]->rot.z);

			if (F_COMP_FLAGS::OBB & m_enemyComps[entityName]->compFlags)
			{
				bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
				bbc->Init();
				Physics::GetInstance().AddCollisionEntity(ent);
			}
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add this enemy");
		}
	}
}

void EnemyHandler::AddMultipleEnemies(std::string name)
{

}
