#include "EnemyFactory.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Components/HealthComponent.h"
#include "Components/EnemyComponent.h"
#include "Misc/EngineRand.h"

EnemyFactory::EnemyFactory()
{
	m_MaxEnemies = 20;
	m_SpawnCooldown = 5;
	m_MinimumDistanceToPlayer = 100;
	m_SpawnTimer = 0.0f;
	m_RandGen.SetSeed(time(NULL));
}

EnemyFactory::EnemyFactory(Scene* scene)
{
	m_pScene = scene;
	m_MaxEnemies = 20;
	m_SpawnCooldown = 5;
	m_MinimumDistanceToPlayer = 1;
	m_SpawnTimer = 0.0f;
	m_RandGen.SetSeed(time(NULL));
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

Entity* EnemyFactory::AddEnemy(const std::string& entityName, EnemyComps* comps)
{
	for (auto pair : m_EnemyComps)
	{
		// An entity with this m_Name already exists
		// so make use of the overloaded version of this function
		if (pair.first == entityName)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Enemy of this type \"%s\" already exists! Overloaded funtion will be used instead!\n", entityName.c_str());
			return AddExistingEnemyWithChanges(entityName, comps->pos, comps->compFlags, comps->aiFlags, comps->scale, comps->rot);
		}
	}	
	EnemyComps* enemy = new EnemyComps;
	m_EnemyComps[entityName] = enemy;

	enemy->enemiesOfThisType++;
	enemy->compFlags = comps->compFlags;
	enemy->aiFlags = comps->aiFlags;
	enemy->scale = comps->scale;
	enemy->rot = comps->rot;
	enemy->model = comps->model;
	enemy->targetName = comps->targetName;
	enemy->hp = comps->hp;
	enemy->sound3D = comps->sound3D;
	enemy->detectionRad = comps->detectionRad;
	enemy->attackingDist = comps->attackingDist;
	enemy->attackInterval = comps->attackInterval;
	enemy->meleeAttackDmg = comps->meleeAttackDmg;
	enemy->movementSpeed = comps->movementSpeed;

	enemy->dim = comps->model->GetModelDim();

	return Add(entityName, enemy);
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
			enemy->pos = pos;
			std::string name = entityName + std::to_string(enemy->enemiesOfThisType);
			enemy->enemiesOfThisType++;

			return Add(name, enemy);
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
			if (compFlags != UINT_MAX)
			{
				enemy->compFlags = compFlags;
			}

			if (aiFlags != UINT_MAX)
			{
				enemy->aiFlags = aiFlags;
			}

			if (scale != FLT_MAX)
			{
				enemy->scale = scale;
			}

			if (rot.x != FLT_MAX)
			{
				enemy->rot = rot;
			}

			if (hp != INT_MAX)
			{
				enemy->hp = hp;
			}

			enemy->pos = pos;

			return Add(name, enemy);
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
			return nullptr;
		}
	}
}

Entity* EnemyFactory::Add(const std::string& entityName, EnemyComps* comps)
{
	Entity* ent = m_pScene->AddEntity(entityName);

	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	component::CollisionComponent* cc = nullptr;
	component::AiComponent* ai = nullptr;
	component::Audio3DEmitterComponent* ae = nullptr;
	component::EnemyComponent* ec = nullptr;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();
	ent->AddComponent<component::HealthComponent>(hp);
	ec = ent->AddComponent<component::EnemyComponent>(this);
	Entity* target = m_pScene->GetEntity(aiTarget);
	double3 targetDim = target->GetComponent<component::ModelComponent>()->GetModelDim();
	float targetScale = target->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().z;
	if (target != nullptr)
	{
		ai = ent->AddComponent<component::AiComponent>(target, aiFlags, aiDetectionRadius, (dim.z * scale * 0.5) + (targetDim.z * targetScale * 0.5) + aiAttackingDistance);
		ai->SetAttackInterval(aiAttackInterval);
		ai->SetMeleeAttackDmg(aiMeleeAttackDmg);
		ai->SetScene(m_pScene);
	}
	ae = ent->AddComponent<component::Audio3DEmitterComponent>();
	ae->AddVoice(comps->sound3D);

	mc->SetModel(comps->model);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	Transform* t = tc->GetTransform();
	t->SetPosition(comps->pos.x, comps->pos.y, comps->pos.z);
	t->SetScale(comps->scale);
	t->SetRotationX(comps->rot.x);
	t->SetRotationY(comps->rot.y);
	t->SetRotationZ(comps->rot.z);
	t->SetVelocity(comps->movementSpeed * 0.5);

	if (comps->compFlags & F_COMP_FLAGS::CAPSULE_COLLISION)
	{
		cc = ent->AddComponent<component::CapsuleCollisionComponent>(1.0, comps->dim.z / 2.0, comps->dim.y - comps->dim.z, 0.01, 0.5, false);
	}
	else if (comps->compFlags & F_COMP_FLAGS::SPHERE_COLLISION)
	{
		cc = ent->AddComponent<component::SphereCollisionComponent>(1.0, comps->dim.y / 2.0, 1.0, 0.0);
	}
	else if (comps->compFlags & F_COMP_FLAGS::CUBE_COLLISION)
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(1.0, comps->dim.x / 2.0, comps->dim.y / 2.0, comps->dim.z / 2.0, 0.01, 0.5, false);
	}
	else
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(0.0, 0.0, 0.0, 0.0);
	}

	if (F_COMP_FLAGS::OBB & comps->compFlags)
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

EnemyComps* EnemyFactory::DefineEnemy(const std::string& entityName, EnemyComps* comps)
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
	enemy->compFlags = comps->compFlags;
	enemy->aiFlags = comps->aiFlags;
	enemy->scale = comps->scale;
	enemy->rot = comps->rot;
	enemy->model = comps->model;
	enemy->targetName = comps->targetName;
	enemy->hp = comps->hp;
	enemy->sound3D = comps->sound3D;
	enemy->detectionRad = comps->detectionRad;
	enemy->attackingDist = comps->attackingDist;
	enemy->attackInterval = comps->attackInterval;
	enemy->meleeAttackDmg = comps->meleeAttackDmg;
	enemy->movementSpeed = comps->movementSpeed;
	enemy->dim = comps->model->GetModelDim();

	return enemy;
}

void EnemyFactory::AddEnemyToList(Entity* enemy)
{
	m_Enemies.push_back(enemy);
}

void EnemyFactory::RemoveEnemyFromList(Entity* enemy)
{
	for (auto enemyInList = m_Enemies.begin(); enemyInList != m_Enemies.end(); ++enemyInList)
	{
		if (*enemyInList == enemy)
		{
			m_Enemies.erase(enemyInList);
			return;
		}
	}
	Log::PrintSeverity(Log::Severity::WARNING, "Tried to erase enemy that does not exist!\n");
}

void EnemyFactory::SetMaxNrOfEnemies(unsigned int val)
{
	m_MaxEnemies = val;
}

void EnemyFactory::SetSpawnCooldown(float val)
{
	m_SpawnCooldown = val;
}

void EnemyFactory::SetMinDistanceFromPlayer(float val)
{
	m_MinimumDistanceToPlayer = val;
}

void EnemyFactory::Update(double dt)
{
	m_SpawnTimer += dt;
	if (m_SpawnCooldown <= m_SpawnTimer)
	{
		std::vector<int> eligblePoints;
		float3 playerPos = m_pScene->GetEntity("player")->GetComponent<component::TransformComponent>()->GetTransform()->GetRenderPositionFloat3();
		for (int i = 0; i < m_SpawnPoints.size(); i++)
		{
			float distToPlayer = (m_SpawnPoints[i] - playerPos).length();
			if (distToPlayer > m_MinimumDistanceToPlayer)
			{
				eligblePoints.push_back(i);
			}
		}
		unsigned int point = m_RandGen.Rand(0, eligblePoints.size());

		unsigned int toSpawn = (m_MaxEnemies - m_Enemies.size()) / 2;

		for (unsigned int i = 0; i < toSpawn; ++i)
		{
			SpawnEnemy("enemyZombie", eligblePoints[point]);
		}
		m_SpawnTimer = 0.0;
	}
}
