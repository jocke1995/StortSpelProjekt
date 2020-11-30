#include "EnemyFactory.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Components/HealthComponent.h"
#include "Components/EnemyComponent.h"
#include "Misc/EngineRand.h"
#include "Physics/CollisionCategories/EnemyCollisionCategory.h"

EnemyFactory& EnemyFactory::GetInstance()
{
	static EnemyFactory instance;
	return instance;
}

EnemyFactory::EnemyFactory()
{
	m_Level = 0;
	m_MaxEnemies = 30;
	m_LevelMaxEnemies = 20;
	m_EnemiesKilled = 0;
	m_EnemiesToSpawn = 0;
	m_EnemySlotsLeft = m_LevelMaxEnemies;
	m_SpawnCooldown = 1;
	m_MinimumDistanceToPlayer = 10;
	m_SpawnTimer = 0.0f;
	m_RandGen.SetSeed(time(NULL));
	EventBus::GetInstance().Subscribe(this, &EnemyFactory::onSceneSwitch);
	EventBus::GetInstance().Subscribe(this, &EnemyFactory::enemyDeath);
	EventBus::GetInstance().Subscribe(this, &EnemyFactory::levelDone);
	EventBus::GetInstance().Subscribe(this, &EnemyFactory::onRoundStart);
	EventBus::GetInstance().Subscribe(this, &EnemyFactory::onResetGame);
}
EnemyFactory::~EnemyFactory()
{
	EventBus::GetInstance().Unsubscribe(this, &EnemyFactory::onSceneSwitch);
	EventBus::GetInstance().Unsubscribe(this, &EnemyFactory::enemyDeath);
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
	enemy->hpBase = comps->hp;
	enemy->sound3D = comps->sound3D;
	enemy->detectionRad = comps->detectionRad;
	enemy->attackingDist = comps->attackingDist;
	enemy->attackInterval = comps->attackInterval;
	enemy->meleeAttackDmg = comps->meleeAttackDmg;
	enemy->meleeAttackDmgBase = comps->meleeAttackDmg;
	enemy->movementSpeedBase = comps->movementSpeed;
	enemy->movementSpeed = comps->movementSpeed;
	enemy->dim = comps->model->GetModelDim();
	enemy->isRanged = comps->isRanged;
	enemy->projectileModel = comps->projectileModel;
	enemy->rangeAttackDmg = comps->rangeAttackDmg;
	enemy->rangeAttackDmgBase = comps->rangeAttackDmg;
	enemy->rangeVelocity = comps->rangeVelocity;

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
	}

	Log::PrintSeverity(Log::Severity::WARNING, "Insuficient input in parameters to add new type of enemy!\n");
	return nullptr;
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
	}

	Log::PrintSeverity(Log::Severity::WARNING, "Inssuficient input in parameters to add new type of enemy!\n");
	return nullptr;
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
	component::RangeEnemyComponent* rangeEnemyComp = nullptr;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>(comps->invertDirection);
	ent->AddComponent<component::HealthComponent>(comps->hp);
	ec = ent->AddComponent<component::EnemyComponent>(this);
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
	t->SetVelocity(comps->movementSpeed);
	t->UpdateWorldMatrix();

	Entity* target = m_pScene->GetEntity(comps->targetName);
	double3 targetDim = target->GetComponent<component::ModelComponent>()->GetModelDim();
	float targetScale = target->GetComponent<component::TransformComponent>()->GetTransform()->GetScale().z;
	if (target != nullptr)
	{
		ai = ent->AddComponent<component::AiComponent>(target, comps->aiFlags, comps->detectionRad, (comps->dim.z * comps->scale * 0.5) + (targetDim.z * targetScale * 0.5) + comps->attackingDist);
		ai->SetAttackInterval(comps->attackInterval);
		ai->SetAttackSpeed(comps->attackSpeed);
		ai->SetMeleeAttackDmg(comps->meleeAttackDmg);
		ai->SetScene(m_pScene);
		if (comps->isRanged)
		{
			rangeEnemyComp = ent->AddComponent<component::RangeEnemyComponent>(&SceneManager::GetInstance(), m_pScene, comps->projectileModel, 0.3, comps->rangeAttackDmg, comps->rangeVelocity);
			rangeEnemyComp->SetAttackInterval(comps->attackInterval);
			ai->SetRangedAI();
		}
	}

	tc->SetTransformOriginalState();
	if (comps->compFlags & F_COMP_FLAGS::CAPSULE_COLLISION)
	{
		cc = ent->AddComponent<component::CapsuleCollisionComponent>(comps->mass, comps->dim.z / 2.0, comps->dim.y - comps->dim.z, 0.01, 0.0, false);
	}
	else if (comps->compFlags & F_COMP_FLAGS::SPHERE_COLLISION)
	{
		cc = ent->AddComponent<component::SphereCollisionComponent>(comps->mass, comps->dim.y / 2.0, 1.0, 0.0);
	}
	else if (comps->compFlags & F_COMP_FLAGS::CUBE_COLLISION)
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(comps->mass, comps->dim.x / 2.0, comps->dim.y / 2.0, comps->dim.z / 2.0, 0.01, 0.0, false);
	}
	else
	{
		cc = ent->AddComponent<component::CubeCollisionComponent>(0.0, 0.0, 0.0, 0.0);
	}

	if (F_COMP_FLAGS::OBB & comps->compFlags)
	{
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		bbc->AddCollisionCategory<EnemyCollisionCategory>();
	}

	m_pScene->InitDynamicEntity(ent);
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

void EnemyFactory::ClearSpawnPoints()
{
	m_SpawnPoints.clear();
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
	enemy->hpBase = comps->hp;
	enemy->sound3D = comps->sound3D;
	enemy->detectionRad = comps->detectionRad;
	enemy->attackingDist = comps->attackingDist;
	enemy->attackInterval = comps->attackInterval;
	enemy->meleeAttackDmg = comps->meleeAttackDmg;
	enemy->meleeAttackDmgBase = comps->meleeAttackDmg;
	enemy->movementSpeed = comps->movementSpeed;
	enemy->movementSpeedBase = comps->movementSpeed;
	enemy->dim = comps->model->GetModelDim();
	enemy->isRanged = comps->isRanged;
	enemy->projectileModel = comps->projectileModel;
	enemy->rangeAttackDmg = comps->rangeAttackDmg;
	enemy->rangeAttackDmgBase = comps->rangeAttackDmg;
	enemy->rangeVelocity = comps->rangeVelocity;
	enemy->invertDirection = comps->invertDirection;

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

void EnemyFactory::SetActive(bool active)
{
	m_IsActive = active;
}

void EnemyFactory::Update(double dt)
{
	if (m_IsActive)
	{
		//m_EnemiesToSpawn hold how many enemies are left to spawn
		//This exist to stagger the spawning of enemies by one frame to spread the load over multiple frames
		if (m_EnemiesToSpawn == 0  && m_EnemySlotsLeft > 0)
		{
			m_SpawnTimer += dt / m_Enemies.size();
			if (m_SpawnCooldown <= m_SpawnTimer)
			{
				m_SpawnTimer = 0.0;
				m_EnemiesToSpawn = 3 + m_Level / 3.0f;

				if (m_EnemiesToSpawn > m_EnemySlotsLeft)
				{
					m_EnemiesToSpawn = m_EnemySlotsLeft;
				}
			}
		} 
		else if(m_EnemySlotsLeft > 0)
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
			int spawnNumber = m_RandGen.Rand(1, 100);
			int spawnChance = 0;
			bool spawnDefault = true;
			for (auto enemy : m_EnemyComps)
			{
				if (enemy.second != nullptr)
				{
					spawnChance += enemy.second->spawnChance;
					if (spawnNumber <= spawnChance)
					{
						SpawnEnemy(enemy.first, eligblePoints[point]);
						spawnDefault = false;
						break;
					}
				}
			}
			if (spawnDefault)
			{
				SpawnEnemy("enemyZombie", eligblePoints[point]);
			}

			m_EnemiesToSpawn--;
			m_EnemySlotsLeft--;
		}
	}
}

void EnemyFactory::enemyDeath(Death* evnt)
{
	if (strcmp(evnt->ent->GetName().substr(0, 5).c_str(), "enemy") == 0)
	{
		m_EnemiesKilled++;

		Entity* enemyGui = m_pScene->GetEntity("enemyGui");
		if (enemyGui != nullptr)
		{
			enemyGui->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(std::to_string(m_EnemiesKilled) + "/" + std::to_string(m_LevelMaxEnemies), "enemyGui");
		}

		//If we have reached the kill goal we are done with the level and should do anything coming from that
		if (m_EnemiesKilled >= m_LevelMaxEnemies)
		{
			EventBus::GetInstance().Publish(&LevelDone());
		}
	}
}

void EnemyFactory::levelDone(LevelDone* evnt)
{
	Entity* teleport = m_pScene->GetEntity("teleporter");
	if (teleport != nullptr)
	{
		teleport->GetComponent<component::TransformComponent>()->GetTransform()->SetPosition(0.0f, 1.0f, 0.0f);
	}
}

void EnemyFactory::onSceneSwitch(SceneChange* evnt)
{
	if (evnt->m_NewSceneName == "ShopScene" || evnt->m_NewSceneName == "gameOverScene" || evnt->m_NewSceneName == "MainMenuScene")
	{
		m_IsActive = false;
	}
	else
	{
		Entity* teleport = m_pScene->GetEntity("teleporter");
		teleport->GetComponent<component::TransformComponent>()->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	}
}

void EnemyFactory::onRoundStart(RoundStart* evnt)
{
	m_IsActive = true;
	m_SpawnTimer = 0.0f;
	m_EnemiesKilled = 0;


	//Scaling difficulty
	m_LevelMaxEnemies = 20 + 2 * m_Level;
	m_EnemySlotsLeft = m_LevelMaxEnemies;

	// melee
	if (m_EnemyComps["enemyZombie"] != nullptr)
	{
		m_EnemyComps["enemyZombie"]->hp = m_EnemyComps["enemyZombie"]->hpBase + 15 * m_Level;
		m_EnemyComps["enemyZombie"]->meleeAttackDmg = m_EnemyComps["enemyZombie"]->meleeAttackDmgBase + 5 * m_Level;
	}

	// meelee quick
	if (m_EnemyComps["enemySpider"] != nullptr)
	{
		m_EnemyComps["enemySpider"]->hp = m_EnemyComps["enemySpider"]->hpBase + 5 * m_Level;
		m_EnemyComps["enemySpider"]->meleeAttackDmg = m_EnemyComps["enemySpider"]->meleeAttackDmgBase + 2 * m_Level;
		if (m_Level > 0)
		{
			if (m_EnemyComps["enemySpider"]->spawnChance < 20)
			{
				m_EnemyComps["enemySpider"]->spawnChance += 10;
			}
		}
	}

	// ranged
	if (m_EnemyComps["enemyDemon"] != nullptr)
	{
		m_EnemyComps["enemyDemon"]->hp = m_EnemyComps["enemyDemon"]->hpBase + 10 * m_Level;
		m_EnemyComps["enemyDemon"]->rangeAttackDmg = m_EnemyComps["enemyDemon"]->rangeAttackDmgBase + 5 * m_Level;
		if (m_Level > 0)
		{
			if (m_EnemyComps["enemyDemon"]->spawnChance < 40)
			{
				m_EnemyComps["enemyDemon"]->spawnChance += 5;
			}
		}
	}


	Entity* enemyGui = m_pScene->GetEntity("enemyGui");
	if (enemyGui != nullptr)
	{
		enemyGui->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("0/" + std::to_string(m_LevelMaxEnemies), "enemyGui");
	}
	++m_Level;
}

void EnemyFactory::onResetGame(ResetGame* evnt)
{
	m_Level = 0;
	m_EnemyComps["enemyDemon"]->spawnChance = 0;
	m_EnemyComps["enemyZombie"]->spawnChance = 0;
	m_EnemyComps["enemySpider"]->spawnChance = 0;
}
