#ifndef ENEMY_HANDLER_H
#define ENEMY_HANDLER_H

#include "Enemy.h"

class EnemyHandler
{
public:
	EnemyHandler();
	~EnemyHandler();


	void AddEnemy(std::string name);
	// Should add multiple of the same type of enemy
	void AddMultipleEnemies(std::string name);


private:

};

#endif