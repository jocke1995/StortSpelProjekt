#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "EngineMath.h"
#include "Core.h"
#include <map>

class Scene;
class Component;
class Model;
class Entity;

enum F_COMP_FLAGS
{
	OBB = BIT(1),
};

// saves the info about a type of enemy
// so that it can be recreated easier
struct EnemyComps
{
	unsigned int compFlags;
	Model* model;
	float3 pos;
	float3 rot;
	float scale;
	int enemiesOfThisType = 0;
};

class EnemyFactory
{
public:
	EnemyFactory(Scene* scene);
	~EnemyFactory();

	// used when you want to add a new type of enemy to the scene 
	// flag is used for selecting which specific components are needed for this enemy e.g. OBB. 
	// Components that enemies have as default won't have a flag
	// will allways draw_opaque and give shadows
	Entity* AddEnemy(std::string entityName, Model* model, float3 pos, unsigned int flag = 0, float scale = 1, float3 rot = { 0, 0, 0 });
	// used if you want to add an already existing enemy type to the scene with only the pos being different
	Entity* AddExistingEnemy(std::string entityName, float3 pos);
	// used if you want to add an already existing enemy but change some of the values; flag, scale or rot.
	// if any of those 3 values are not written in they will use the values stored in the struct of this enemy type
	// if you want to for example only change the scale then write UINT_MAX as flag, then normal values for scale
	// to skipp scale and write to rot the equvalent would be FLT_MAX
	Entity* AddExistingEnemyWithChanges(std::string entityName, float3 pos, unsigned int flag = UINT_MAX, float scale = FLT_MAX, float3 rot = { FLT_MAX , FLT_MAX , FLT_MAX });




private:
	Scene* m_pScene = nullptr;
	std::map<std::string, EnemyComps*> m_EnemyComps;
};

#endif