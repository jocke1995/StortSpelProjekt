#ifndef POISON_DEBUFF_H
#define POISON_DEBUFF_H

#include "../../Engine/src/ECS/Components/Component.h"
#include "EngineMath.h"

class Entity;
namespace component
{
	class PoisonDebuff : public Component
	{
	public:
		PoisonDebuff(Entity* parent, int damagePerTick, int ticks, double tickDuration, float percentageSlow);
		~PoisonDebuff();

		void Reset(int damagePerTick, int ticks, double tickDuration, float percentageSlow);

		void Update(double dt);

	private:
		int m_Damage;
		int m_Ticks;
		double m_TickDuration;
		double m_TickTimer;
		float m_Slow;
	};


}




#endif // !POISON_DEBUFF_H
