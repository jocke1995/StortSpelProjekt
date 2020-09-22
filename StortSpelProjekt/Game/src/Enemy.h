#pragma once
#ifndef ENEMY_H
#define ENEMY_H
#include "..\..\Engine\src\ECS\Entity.h"

class Enemy : public Entity
{
public:
	Enemy(std::string name);
	~Enemy();

	void Update(double dt);

private:


};

#endif