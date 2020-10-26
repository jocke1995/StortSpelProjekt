#include "EnemyFactory.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Components/HealthComponent.h"
#include "Misc/EngineRand.h"
EnemyFactory::EnemyFactory()
{
	m_RandGen.SetSeed(time(NULL));
}

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

void EnemyFactory::SetScene(Scene* scene)
{
	m_pScene = scene;
}

Entity* EnemyFactory::AddEnemy(const std::string& entityName, Model* model, int hp, float3 pos, const std::wstring& sound3D, unsigned int compFlags, unsigned int aiFlags, float scale, float3 rot, const std::string& aiTarget, float aiDetectionRadius, float aiAttackingDistance, float aiAttackInterval)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so make use of the overloaded version of this function
		if (pair.first == entityName)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Enemy of this type \"%s\" already exists! Overloaded funtion will be used instead!\n", entityName.c_str());
			return AddExistingEnemyWithChanges(entityName, pos, compFlags, aiFlags, scale, rot);
		}
	}	
	EnemyComps* enemy = new EnemyComps;
	m_EnemyComps[entityName] = enemy;

	enemy->enemiesOfThisType++;
	enemy->compFlags = compFlags;
	enemy->aiFlags = aiFlags;
	enemy->scale = scale;
	enemy->rot = rot;
	enemy->model = model;
	enemy->targetName = aiTarget;
	enemy->hp = hp;
	enemy->sound3D = sound3D;
	enemy->detectionRad = aiDetectionRadius;
	enemy->attackingDist = aiAttackingDistance;
	enemy->attackInterval = aiAttackInterval;

	enemy->dim = model->GetModelDim();

	return Add(entityName, model, hp, pos, sound3D, compFlags, aiFlags, enemy->dim, scale, rot, aiTarget, aiDetectionRadius, aiAttackingDistance, aiAttackInterval);
}

Entity* EnemyFactory::AddExistingEnemy(const std::string& entityName, float3 pos)
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

			return Add(name, enemy->model, enemy->hp, pos, enemy->sound3D, enemy->compFlags, enemy->aiFlags, enemy->dim, enemy->scale, enemy->rot, enemy->targetName, enemy->detectionRad, enemy->attackingDist, enemy->attackInterval);
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Insuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

Entity* EnemyFactory::AddExistingEnemyWithChanges(const std::string& entityName, float3 pos, unsigned int compFlags, unsigned int aiFlags, float scale, float3 rot, int hp)
{
	for (auto pair : m_EnemyComps)
	{
		std::string name = entityName + std::to_string(m_EnemyComps[entityName]->enemiesOfThisType);
		EnemyComps* enemy = m_EnemyComps[entityName];
		enemy->enemiesOfThisType++;

		// An entity with this m_Name already exists
		// so create a new onen of the same type
		if (pair.first == entityName)
		{
			// if any of the inputs are not default values use them
			// otherwise use the values from the struct
			unsigned int newCompFlags;
			if (compFlags != UINT_MAX)
			{
				newCompFlags = compFlags;
			}
			else
			{
				newCompFlags = enemy->compFlags;
			}
			unsigned int newAiFlags;
			if (aiFlags != UINT_MAX)
			{
				newAiFlags = aiFlags;
			}
			else
			{
				newAiFlags = enemy->aiFlags;
			}
			float newScale;
			if (scale != FLT_MAX)
			{
				newScale = scale;
			}
			else
			{
				newScale = enemy->scale;
			}
			float3 newRot;
			if (rot.x != FLT_MAX)
			{
				newRot = rot;
			}
			else
			{
				newRot = enemy->rot;
			}
			int newHP;
			if (hp != INT_MAX)
			{
				newHP = hp;
			}
			else
			{
				newHP = enemy->hp;
			}

			return Add(name, enemy->model, newHP, pos, enemy->sound3D, newCompFlags, newAiFlags, enemy->dim, newScale, newRot, enemy->targetName, enemy->detectionRad, enemy->attackingDist, enemy->attackInterval);
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

Entity* EnemyFactory::Add(const std::string& name, Model* model, int hp, float3 pos, const std::wstring& sound3D, unsigned int compFlags, unsigned int aiFlags, double3 dim, float scale, float3 rot, const std::string& aiTarget, float aiDetectionRadius, float aiAttackingDistance, float aiAttackInterval)
{
	Entity* ent = m_pScene->AddEntity(name);

	m_Enemies.push_back(ent);

	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	component::CollisionComponent* cc = nullptr;
	component::AiComponent* ai = nullptr;
	component::Audio3DEmitterComponent* ae = nullptr;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();
	ent->AddComponent<component::HealthComponent>(hp);

	Entity* target = m_pScene->GetEntity(aiTarget);
	if (target != nullptr)
	{
		ai = ent->AddComponent<component::AiComponent>(target, aiFlags, aiDetectionRadius, aiAttackingDistance);
		ai->SetAttackInterval(aiAttackInterval);
		ai->SetScene(m_pScene);
	}
	ae = ent->AddComponent<component::Audio3DEmitterComponent>();
	ae->AddVoice(sound3D);

	mc->SetModel(model);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	Transform* t = tc->GetTransform();
	t->SetPosition(pos.x, pos.y, pos.z);
	t->SetScale(scale);
	t->SetRotationX(rot.x);
	t->SetRotationY(rot.y);
	t->SetRotationZ(rot.z);
	t->SetVelocity(BASE_VEL * 0.5);

	if (compFlags & F_COMP_FLAGS::CAPSULE_COLLISION)
	{
		cc = ent->AddComponent<component::CapsuleCollisionComponent>(1.0, dim.z / 2.0, dim.y - dim.z, 0.01, 0.5, false);
	}
	else if (compFlags & F_COMP_FLAGS::SPHERE_COLLISION)
	{
		cc = ent->AddComponent<component::SphereCollisionComponent>(1.0, dim.y / 2.0, 1.0, 0.0);
	}
	else if (compFlags & F_COMP_FLAGS::CUBE_COLLISION)
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(1.0, dim.x / 2.0, dim.y / 2.0, dim.z / 2.0, 0.01, 0.5, false);
	}
	else
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(0.0, 0.0, 0.0, 0.0);
	}

	if (F_COMP_FLAGS::OBB & compFlags)
	{
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		Physics::GetInstance().AddCollisionEntity(ent);
	}

	SceneManager::GetInstance().AddEntity(ent, m_pScene);
	return ent;
}

std::vector<Entity*>* EnemyFactory::GetAllEnemies()
{
	return &m_Enemies;
}

void EnemyFactory::AddSpawnPoint(const float3& point)
{
	m_SpawnPoints.push_back({ point.x, point.y, point.z });
}

Entity* EnemyFactory::SpawnEnemy(std::string entityName, unsigned int spawnPoint)
{
	return AddExistingEnemy(entityName, m_SpawnPoints[spawnPoint]);
}

Entity* EnemyFactory::SpawnEnemy(std::string entityName)
{
	return SpawnEnemy(entityName, m_RandGen.Rand(0,m_SpawnPoints.size()));
}

EnemyComps* EnemyFactory::DefineEnemy(const std::string& entityName, Model* model, int hp, const std::wstring& sound3D, unsigned int compFlags, unsigned int aiFlags, float scale, float3 rot, const std::string& aiTarget, float aiDetectionRadius, float aiAttackingDistance, float aiAttackInterval)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so make use of the overloaded version of this function
		if (pair.first == entityName)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Enemy of this type \"%s\" already exists!\n", entityName.c_str());
			return pair.second;
		}
	}
	EnemyComps* enemy = new EnemyComps;
	m_EnemyComps[entityName] = enemy;

	enemy->enemiesOfThisType = 0;
	enemy->compFlags = compFlags;
	enemy->aiFlags = aiFlags;
	enemy->scale = scale;
	enemy->rot = rot;
	enemy->model = model;
	enemy->targetName = aiTarget;
	enemy->hp = hp;
	enemy->sound3D = sound3D;
	enemy->detectionRad = aiDetectionRadius;
	enemy->attackingDist = aiAttackingDistance;
	enemy->attackInterval = aiAttackInterval;
	enemy->dim = model->GetModelDim();
	return enemy;
}

void EnemyFactory::SetEnemyTypeMaxHealth(const std::string& enemyName, int hp)
{
	m_EnemyComps[enemyName]->hp = hp;

	for (Entity* enemy : m_Enemies)
	{
		if (enemy->GetName().find(enemyName) != std::string::npos)
		{
			enemy->GetComponent<component::HealthComponent>()->SetMaxHealth(hp);
		}
	}
}

