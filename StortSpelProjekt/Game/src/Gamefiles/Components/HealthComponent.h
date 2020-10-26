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
		HealthComponent(Entity* parent, int hp, float removalTime = -1);
		~HealthComponent();
		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();


		// will publish a death event if hp reaches 0 or bellow
		void SetHealth(int hp);
		// Meant to be used for taking damage (input negative numbers)
		// or maybe healing (input positive numbers)
		// will publish a death event if hp reaches 0 or bellow
		void ChangeHealth(int hpChange);

		void TakeDamage(int damage);

		void ChangeFlatDamageReduction(int flatDamageReduction);
		void ChangeProcentileDamageReduction(float procentileDamageReduction);

		int GetHealth() const;

		int GetMaxHealth() const;
		void SetMaxHealth(int newHealth);
		void ChangeMaxHealth(int hpChange);

	private:
		int m_FlatDamageReduction;
		float m_ProcentileDamageReduction;
		int m_Health;
		int m_MaxHealth;
		bool m_Dead = false;
		float m_DeathDuration = 0.0f;
		float m_RemovalTimer;
		// temporary function that decreases health by 1 everytime collision happens
		// void decreaseHealth(Collision* event);
	};


}




#endif // !HEALTH_COMPONENT_H
