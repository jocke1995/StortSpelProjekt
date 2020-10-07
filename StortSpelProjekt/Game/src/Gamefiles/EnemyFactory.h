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
	int hp;
	std::string targetName;
	int enemiesOfThisType = 0;
	std::wstring sound3D;
	std::wstring sound2D;
};

class EnemyFactory
{
public:
	EnemyFactory(Scene* scene);
	~EnemyFactory();

	// Used when you want to add a new type of enemy to the scene .
	// Flag is used for selecting which specific components are needed for this enemy e.g. OBB. 
	// Components that enemies have as default won't have a flag.
	// Will allways draw_opaque and give shadows.
	Entity* AddEnemy(std::string entityName, Model* model, int hp, float3 pos, std::wstring sound3D, unsigned int flag = 0, float scale = 1, float3 rot = { 0, 0, 0 }, std::string aiTarget = "player");
	// used if you want to add an already existing enemy type to the scene with only the pos being different
	Entity* AddExistingEnemy(std::string entityName, float3 pos);
	// Used if you want to add an already existing enemy but change some of the values; flag, scale, rot or hp.
	// If any of those values are not written in they will use the values stored in the struct of this enemy type.
	// If you want to for example only change the scale then write UINT_MAX as flag, then normal values for scale.
	// To skipp scale and write to rot the equvalent would be FLT_MAX.
	Entity* AddExistingEnemyWithChanges(std::string entityName, float3 pos, unsigned int flag = UINT_MAX, float scale = FLT_MAX, float3 rot = { FLT_MAX , FLT_MAX , FLT_MAX }, int hp = INT_MAX);




private:
	Scene* m_pScene = nullptr;
	std::map<std::string, EnemyComps*> m_EnemyComps;
};

#endif