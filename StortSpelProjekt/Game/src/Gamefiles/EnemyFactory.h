#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "EngineMath.h"
#include "Core.h"
#include <map>
#include <vector>

class Scene;
class Component;
class Model;
class Entity;

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
	unsigned int compFlags;
	unsigned int aiFlags;
	Model* model;
	float3 pos;
	float3 rot;
	float scale;
	int hp;
	std::string targetName;
	int enemiesOfThisType = 0;
	std::wstring sound3D;
	double3 dim;
	float detectionRad;
	float attackingDist;
};

class EnemyFactory
{
public:
	EnemyFactory();
	EnemyFactory(Scene* scene);
	~EnemyFactory();

	void SetScene(Scene* scene);

	// Used when you want to add a new type of enemy to the scene .
	// Flag is used for selecting which specific components are needed for this enemy e.g. OBB. 
	// Components that enemies have as default won't have a flag.
	// Will allways draw_opaque and give shadows.
	Entity* AddEnemy(std::string entityName, Model* model, int hp, float3 pos, std::wstring sound3D, unsigned int compFlags = 0, unsigned int aiFlags = 0, float scale = 1, float3 rot = { 0, 0, 0 }, std::string aiTarget = "player", float aiDetectionRadius = 25.0f, float aiAttackingDistance = 3.5f);
	// used if you want to add an already existing enemy type to the scene with only the pos being different
	Entity* AddExistingEnemy(std::string entityName, float3 pos);
	// Used if you want to add an already existing enemy but change some of the values; flag, scale, rot or hp.
	// If any of those values are not written in they will use the values stored in the struct of this enemy type.
	// If you want to for example only change the scale then write UINT_MAX as flag, then normal values for scale.
	// To skipp scale and write to rot the equvalent would be FLT_MAX.
	Entity* AddExistingEnemyWithChanges(std::string entityName, float3 pos, unsigned int compFlags = UINT_MAX, unsigned int aiFlags = UINT_MAX, float scale = FLT_MAX, float3 rot = { FLT_MAX , FLT_MAX , FLT_MAX }, int hp = INT_MAX);
	// Helper function for adding an enemy
	Entity* Add(std::string entityName, Model* model, int hp, float3 pos, std::wstring sound3D, unsigned int flag, unsigned int aiFlags, double3 dim, float scale, float3 rot, std::string aiTarget, float aiDetectionRadius, float aiAttackingDistance);

	std::vector<Entity*>* GetAllEnemies();

private:
	Scene* m_pScene = nullptr;
	std::map<std::string, EnemyComps*> m_EnemyComps;
	std::vector<Entity*> m_Enemies;
};

#endif