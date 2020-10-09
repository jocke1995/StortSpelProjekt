#include "EnemyFactory.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Components/HealthComponent.h"

EnemyFactory::EnemyFactory(Scene* scene)
{
	m_pScene = scene;
}

EnemyFactory::~EnemyFactory()
{
	for (auto pair : m_EnemyComps)
	{
		if (pair.second != nullptr)
		{
			delete pair.second;
		}
	}
	m_EnemyComps.clear();
}

Entity* EnemyFactory::AddEnemy(std::string entityName, Model* model, int hp, float3 pos, std::wstring sound3D, std::wstring sound2D, unsigned int flag, float scale, float3 rot, std::string aiTarget)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so make use of the overloaded version of this function
		if (pair.first == entityName)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Enemy of this type \"%s\" already exists! Overloaded funtion will be used instead!\n", entityName.c_str());
			return AddExistingEnemyWithChanges(entityName, pos, flag, scale, rot);
		}
	}	
	EnemyComps* enemy = new EnemyComps;
	m_EnemyComps[entityName] = enemy;

	enemy->enemiesOfThisType++;
	enemy->compFlags = flag;
	enemy->pos = pos;
	enemy->scale = scale;
	enemy->rot = rot;
	enemy->model = model;
	enemy->targetName = aiTarget;
	enemy->hp = hp;
	enemy->sound3D = sound3D;
	enemy->sound2D = sound2D;

	for (unsigned int i = 0; i < model->GetSize(); i++)
	{
		std::vector<Vertex> modelVertices = *model->GetMeshAt(i)->GetVertices();
		float3 minVertex = { 100.0, 100.0, 100.0 }, maxVertex = { -100.0, -100.0, -100.0 };
		for (unsigned int i = 0; i < modelVertices.size(); i++)
		{
			minVertex.x = Min(minVertex.x, modelVertices[i].pos.x);
			minVertex.y = Min(minVertex.y, modelVertices[i].pos.y);
			minVertex.z = Min(minVertex.z, modelVertices[i].pos.z);

			maxVertex.x = Max(maxVertex.x, modelVertices[i].pos.x);
			maxVertex.y = Max(maxVertex.y, modelVertices[i].pos.y);
			maxVertex.z = Max(maxVertex.z, modelVertices[i].pos.z);
		}
		enemy->dim = { maxVertex.x - minVertex.x, maxVertex.y - minVertex.y, maxVertex.z - minVertex.z };
	}

	return Add(entityName, model, hp, pos, sound3D, sound2D, flag, enemy->dim, scale, rot, aiTarget);
}

Entity* EnemyFactory::AddExistingEnemy(std::string entityName, float3 pos)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so create a new one of the same type
		if (pair.first == entityName)
		{
			EnemyComps* enemy = m_EnemyComps[entityName];
			std::string name = entityName + std::to_string(enemy->enemiesOfThisType);
			enemy->enemiesOfThisType++;

			return Add(name, enemy->model, enemy->hp, pos, enemy->sound3D, enemy->sound2D, enemy->compFlags, enemy->dim, enemy->scale, enemy->rot, enemy->targetName);
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Insuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

Entity* EnemyFactory::AddExistingEnemyWithChanges(std::string entityName, float3 pos, unsigned int flag, float scale, float3 rot, int hp)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so create a new onen of the same type
		if (pair.first == entityName)
		{
			// if any of the inputs are not default values use them
			// otherwise use the values from the struct
			unsigned int newFlag;
			if (flag != UINT_MAX)
			{
				newFlag = flag;
			}
			else
			{
				newFlag = m_EnemyComps[entityName]->compFlags;
			}
			float newScale;
			if (scale != FLT_MAX)
			{
				newScale = scale;
			}
			else
			{
				newScale = m_EnemyComps[entityName]->scale;
			}
			float3 newRot;
			if (rot.x != FLT_MAX)
			{
				newRot = rot;
			}
			else
			{
				newRot = m_EnemyComps[entityName]->rot;
			}
			int newHP;
			if (hp != INT_MAX)
			{
				newHP = hp;
			}
			else
			{
				newHP = m_EnemyComps[entityName]->hp;
			}

			std::string name = entityName + std::to_string(m_EnemyComps[entityName]->enemiesOfThisType);
			EnemyComps* enemy = m_EnemyComps[entityName];
			enemy->enemiesOfThisType++;

			return Add(name, enemy->model, newHP, pos, enemy->sound3D, enemy->sound2D, newFlag, enemy->dim, newScale, newRot, enemy->targetName);
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

Entity* EnemyFactory::Add(std::string name, Model* model, int hp, float3 pos, std::wstring sound3D, std::wstring sound2D, unsigned int flag, float3 dim, float scale, float3 rot, std::string aiTarget)
{
	Entity* ent = m_pScene->AddEntity(name);
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	component::CollisionComponent* cc = nullptr;
	component::AiComponent* ai = nullptr;
	component::Audio3DEmitterComponent* ae = nullptr;
	component::Audio2DVoiceComponent* avc = nullptr;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();
	ent->AddComponent<component::HealthComponent>(hp);

	Entity* target = m_pScene->GetEntity(aiTarget);
	if (target != nullptr)
	{
		bool canJump = flag & F_COMP_FLAGS::CAN_JUMP;
		ai = ent->AddComponent<component::AiComponent>(target, canJump);
	}
	ae = ent->AddComponent<component::Audio3DEmitterComponent>();
	ae->AddVoice(sound3D);
	avc = ent->AddComponent<component::Audio2DVoiceComponent>();
	avc->AddVoice(sound2D);

	mc->SetModel(model);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	tc->GetTransform()->SetScale(scale);
	tc->GetTransform()->SetRotationX(rot.x);
	tc->GetTransform()->SetRotationY(rot.y);
	tc->GetTransform()->SetRotationZ(rot.z);

	if (flag & F_COMP_FLAGS::CAPSULE_COLLISION)
	{
		cc = ent->AddComponent<component::CapsuleCollisionComponent>(1.0, dim.x * scale / 2.0, dim.y / 2.0 * scale, 0.01, 0.5, false);
	}
	else if (flag & F_COMP_FLAGS::SPHERE_COLLISION)
	{
		cc = ent->AddComponent<component::SphereCollisionComponent>(1.0, dim.y * scale / 2.0, 1.0, 0.0);
	}
	else if (flag & F_COMP_FLAGS::CUBE_COLLISION)
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(1.0, dim.x * scale / 2.0, dim.y * scale / 2.0, dim.z * scale / 2.0, 0.01, 0.5, false);
	}
	else
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(0.0, 0.0, 0.0, 0.0);
	}

	if (F_COMP_FLAGS::OBB & flag)
	{
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		Physics::GetInstance().AddCollisionEntity(ent);
	}
	return ent;
}

