#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

#include "../../Engine/src/ECS/Components/Component.h"
#include "EngineMath.h"

struct Death;
class Entity;
namespace component
{
	class HealthComponent : public Component
	{
	public:
		HealthComponent(Entity* parent, int hp);
		~HealthComponent();

		void OnInitScene();
		void OnUnInitScene();


		// will publish a death event if hp reaches 0 or bellow
		void SetHealth(int hp);
		// Meant to be used for taking damage (input negative numbers)
		// or maybe healing (input positive numbers)
		// will publish a death event if hp reaches 0 or bellow
		void ChangeHealth(int hpChange);

		int GetHealth();

		int GetMaxHealth() const;
		void SetMaxHealth(int newHealth);
		void ChangeMaxHealth(int hpChange);

	private:
		int m_Health;
		int m_MaxHealth;
		bool m_Dead = false;
		// temporary function that decreases health by 1 everytime collision happens
		// void decreaseHealth(Collision* event);

		// temporary function that prints when death event is published
		// and m_parent was the entity that died
		void printDeath(Death* event);

	};


}




#endif // !HEALTH_COMPONENT_H
