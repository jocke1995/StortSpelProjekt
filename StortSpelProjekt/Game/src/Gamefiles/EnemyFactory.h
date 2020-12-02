#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "EngineMath.h"
#include "Core.h"
#include "Misc/EngineRand.h"
#include "Misc/EngineRand.h"
#include <map>
#include <vector>

class Scene;
class Component;
class Model;
class Entity;
struct SceneChange;
struct Death;
struct LevelDone;
struct RoundStart;
struct ResetGame;

static int s_EnemyId = 0;

enum F_COMP_FLAGS
{
	OBB = BIT(1),
	CUBE_COLLISION = BIT(2),
	SPHERE_COLLISION = BIT(3),
	CAPSULE_COLLISION = BIT(4),
};

// saves the info about a type of enemy
// so that it can be recreated easier
struct EnemyComps
{
	unsigned int compFlags = 0;
	unsigned int aiFlags = 0;
	unsigned int spawnChance = 0;
	Model* model = nullptr;
	float3 rot = { 0, 0, 0 };
	float3 pos = { 0, 1, 0 };
	float scale = 1.0;
	int hp = 10;
	int hpBase = 10;
	std::string targetName = "player";
	int enemiesOfThisType = 0;
	std::vector<std::wstring> OnHitSounds;
	std::vector<std::wstring> OnGruntSounds;
	double3 dim = double3({ 0.0, 0.0, 0.0 });
	float detectionRad = 25.0f;
	float attackingDist = 0.5f;
	float attackInterval = 1.0f;
	float attackSpeed = 0.2f;
	float meleeAttackDmg = 10.0f;
	float meleeAttackDmgBase = 10.0f;
	float slowAttack = 0.0f;
	float movementSpeed = 30.0f;
	float movementSpeedBase = 30.0f;
	bool isRanged = false;
	Model* projectileModel = nullptr;
	float rangeAttackDmg = 10.0f;
	float rangeAttackDmgBase = 10.0f;
	float rangeVelocity = 50.0f;
	bool invertDirection = false;
	float mass = 1.0f;
};

class EnemyFactory
{
public:
	static EnemyFactory& GetInstance();

	~EnemyFactory();

	void SetScene(Scene* scene);

	// Used when you want to add a new type of enemy to the scene .
	// Flag is used for selecting which specific components are needed for this enemy e.g. OBB. 
	// Components that enemies have as default won't have a flag.
	// Will allways draw_opaque and give shadows.
	Entity* AddEnemy(const std::string& entityName, EnemyComps* comps);
	// used if you want to add an already existing enemy type to the scene with only the pos being different
	Entity* AddExistingEnemy(const std::string& entityName, float3 pos);
	// Used if you want to add an already existing enemy but change some of the values; flag, scale, rot or hp.
	// If any of those values are not written in they will use the values stored in the struct of this enemy type.
	// If you want to for example only change the scale then write UINT_MAX as flag, then normal values for scale.
	// To skipp scale and write to rot the equvalent would be FLT_MAX.
	Entity* AddExistingEnemyWithChanges(const std::string& entityName, float3 pos, unsigned int compFlags = UINT_MAX, unsigned int aiFlags = UINT_MAX, float scale = FLT_MAX, float3 rot = { FLT_MAX , FLT_MAX , FLT_MAX }, int hp = INT_MAX);
	// Helper function for adding an enemy
	Entity* Add(const std::string& entityName, EnemyComps* comps);

	std::vector<Entity*>* GetAllEnemies();
	
	// Adds a spawnpoint.
	void AddSpawnPoint(const float3& point);
	void ClearSpawnPoints();
	// Adds an enemy at the indicated spawnpoint.
	Entity* SpawnEnemy(std::string entityName, unsigned int spawnPoint);
	// Spawns an enemy at random spawnpoint. Assumes that there are atleast two spawnpoints!
	Entity* SpawnEnemy(std::string entityName);
	// Defines an enemy without adding it to the scene.
	EnemyComps* DefineEnemy(const std::string& entityName, EnemyComps* comps);

	// Sets the max health of all enemies of a specific type
	void SetEnemyTypeMaxHealth(const std::string& enemyName, int hp);


	void AddEnemyToList(Entity* enemy);
	void RemoveEnemyFromList(Entity* enemy);

	void SetMaxNrOfEnemies(unsigned int val);
	void SetSpawnCooldown(float val);
	void SetMinDistanceFromPlayer(float val);

	void SetActive(bool active);

	void Update(double dt);
private:
	EnemyFactory();

	void enemyDeath(Death* evnt);
	void levelDone(LevelDone* evnt);
	void onSceneSwitch(SceneChange* evnt);
	void onRoundStart(RoundStart* evnt);
	void onResetGame(ResetGame* evnt);
	Scene* m_pScene = nullptr;
	std::map<std::string, EnemyComps*> m_EnemyComps;
	std::vector<Entity*> m_Enemies;
	std::vector<float3> m_SpawnPoints;
	EngineRand m_RandGen;

	int m_MaxEnemies;
	int m_EnemiesToSpawn;
	int m_LevelMaxEnemies;
	int m_EnemySlotsLeft;
	int m_EnemiesKilled;
	unsigned int m_Level;
	float m_SpawnCooldown;
	float m_SpawnTimer;
	float m_MinimumDistanceToPlayer;

	bool m_IsActive = false;
};

#endif