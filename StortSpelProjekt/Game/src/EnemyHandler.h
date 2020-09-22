#ifndef ENEMY_HANDLER_H
#define ENEMY_HANDLER_H

#include "Enemy.h"
#include "EngineMath.h"
#include <map>
#include <vector>
//#include "ECS/Components/Component.h"
//
//class Entity;
class Scene;
class Component;
class Model;

enum F_COMP_FLAGS
{
	OBB = BIT(1),
};

// saves the info about a type of enemy
// so that it can be recreated easier
struct EnemyComps
{
	//std::vector<std::string> s_Components;

	unsigned int compFlags;
	Model* model;
	float3 pos;
	float3 rot;
	float scale;
	int enemiesOfThisType = 0;
};

class EnemyHandler
{
public:
	EnemyHandler(Scene* scene);
	~EnemyHandler();

	// used when you want to add a new type of enemy to the scene
	// flag is used for selecting which specific components are needed for this enemy
	// e.g. OBB. Components that enemies have as default won't have a flag
	// will allways draw_opaque and give shadows
	void AddEnemy(std::string entityName, Model* model, unsigned int flag, float3 pos, float scale = 1, float3 rot = { 0, 0, 0 });
	// used if you want to add an already existing enemy type to the scene
	void AddEnemy(std::string entityName, float3 pos);
	// Should add multiple of the same type of enemy 
	// NOT DONE
	void AddMultipleEnemies(std::string name);


private:
	Scene* m_pScene = nullptr;
	std::map<std::string, EnemyComps*> m_enemyComps;
};

#endif