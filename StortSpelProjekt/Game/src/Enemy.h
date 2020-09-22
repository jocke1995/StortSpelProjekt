#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "..\ECS\Components\Component.h"
#include "Core.h"

namespace component
{
	class Enemy : public Component
	{
	public:
		Enemy(Entity* parent, std::string name);
		~Enemy();

		void Update(double dt);

	private:


	};
}


#endif