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
	Entity *ent = m_pScene->AddEntity(entityName);
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	component::CollisionComponent* cc = nullptr;
	component::AiComponent* ai = nullptr;
	component::Audio3DEmitterComponent* ae = nullptr;
	component::Audio2DVoiceComponent* avc = nullptr;
	m_EnemyComps[entityName] = new EnemyComps;

	m_EnemyComps[entityName]->enemiesOfThisType++;
	m_EnemyComps[entityName]->compFlags = flag;
	m_EnemyComps[entityName]->pos = pos;
	m_EnemyComps[entityName]->scale = scale;
	m_EnemyComps[entityName]->rot = rot;
	m_EnemyComps[entityName]->model = model;
	m_EnemyComps[entityName]->targetName = aiTarget;
	m_EnemyComps[entityName]->hp = hp;
	m_EnemyComps[entityName]->sound3D = sound3D;
	m_EnemyComps[entityName]->sound2D = sound2D;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();	
	ent->AddComponent<component::HealthComponent>(hp);
	tc = ent->AddComponent<component::TransformComponent>();
	cc = ent->AddComponent<component::CapsuleCollisionComponent>(1.0, 1.0, 2.0);
	Entity* target = m_pScene->GetEntity(aiTarget);
	if (target != nullptr)
	{
		ai = ent->AddComponent<component::AiComponent>(target);
	}
	ae = ent->AddComponent<component::Audio3DEmitterComponent>();
	ae->AddVoice(sound3D);
	avc = ent->AddComponent<component::Audio2DVoiceComponent>();
	avc->AddVoice(sound2D);

	mc->SetModel(m_EnemyComps[entityName]->model/*model*/);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	tc->GetTransform()->SetScale(scale);
	tc->GetTransform()->SetRotationX(rot.x);
	tc->GetTransform()->SetRotationY(rot.y);
	tc->GetTransform()->SetRotationZ(rot.z);
	tc->GetTransform()->SetVelocity(2.5);
	if (F_COMP_FLAGS::OBB & flag)
	{
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		//m_enemyComps[entityName]->s_Components.push_back("");
		Physics::GetInstance().AddCollisionEntity(ent);
	}
	return ent;
}

Entity* EnemyFactory::AddExistingEnemy(std::string entityName, float3 pos)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so create a new one of the same type
		if (pair.first == entityName)
		{
			std::string name = entityName + std::to_string(m_EnemyComps[entityName]->enemiesOfThisType);
			Entity* ent = m_pScene->AddEntity(name);
			m_EnemyComps[entityName]->enemiesOfThisType++;
			component::ModelComponent* mc = nullptr;
			component::TransformComponent* tc = nullptr;
			component::BoundingBoxComponent* bbc = nullptr;
			component::CollisionComponent* cc = nullptr;
			component::AiComponent* ai = nullptr;
			component::Audio3DEmitterComponent* ae = nullptr;
			component::Audio2DVoiceComponent* avc = nullptr;
			
			mc = ent->AddComponent<component::ModelComponent>();
			tc = ent->AddComponent<component::TransformComponent>();
			ent->AddComponent<component::HealthComponent>(m_EnemyComps[entityName]->hp);
			cc = ent->AddComponent<component::CapsuleCollisionComponent>(1.0, 1.0, 2.0);
			Entity* target = m_pScene->GetEntity(m_EnemyComps[entityName]->targetName);
			if (target != nullptr)
			{
				ai = ent->AddComponent<component::AiComponent>(target);
			}
			ae = ent->AddComponent<component::Audio3DEmitterComponent>();
			ae->AddVoice(m_EnemyComps[entityName]->sound3D);
			avc = ent->AddComponent<component::Audio2DVoiceComponent>();
			avc->AddVoice(m_EnemyComps[entityName]->sound2D);


			mc->SetModel(m_EnemyComps[entityName]->model);
			mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
			tc->GetTransform()->SetPosition(pos.x, pos.y , pos.z);
			tc->GetTransform()->SetScale(m_EnemyComps[entityName]->scale);
			tc->GetTransform()->SetRotationX(m_EnemyComps[entityName]->rot.x);
			tc->GetTransform()->SetRotationY(m_EnemyComps[entityName]->rot.y);
			tc->GetTransform()->SetRotationZ(m_EnemyComps[entityName]->rot.z);

			if (F_COMP_FLAGS::OBB & m_EnemyComps[entityName]->compFlags)
			{
				bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
				bbc->Init();
				Physics::GetInstance().AddCollisionEntity(ent);
			}
			return ent;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
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
			Entity* ent = m_pScene->AddEntity(name);
			m_EnemyComps[entityName]->enemiesOfThisType++;
			component::ModelComponent* mc = nullptr;
			component::TransformComponent* tc = nullptr;
			component::BoundingBoxComponent* bbc = nullptr;
			component::CollisionComponent* cc = nullptr;
			component::AiComponent* ai = nullptr;

			mc = ent->AddComponent<component::ModelComponent>();
			tc = ent->AddComponent<component::TransformComponent>();
			ent->AddComponent<component::HealthComponent>(newHP);
			cc = ent->AddComponent<component::CapsuleCollisionComponent>(1.0, 1.0, 2.0);
			Entity* target = m_pScene->GetEntity(m_EnemyComps[entityName]->targetName);
			if (target != nullptr)
			{
				ai = ent->AddComponent<component::AiComponent>(target);
			}


			mc->SetModel(m_EnemyComps[entityName]->model);
			mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
			tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
			tc->GetTransform()->SetScale(newScale);
			tc->GetTransform()->SetRotationX(newRot.x);
			tc->GetTransform()->SetRotationY(newRot.y);
			tc->GetTransform()->SetRotationZ(newRot.z);

			if (F_COMP_FLAGS::OBB & newFlag)
			{
				bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
				bbc->Init();
				Physics::GetInstance().AddCollisionEntity(ent);
			}
			return ent;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

