#ifndef POISON_DEBUFF_H
#define POISON_DEBUFF_H

#include "../../Engine/src/ECS/Components/Component.h"
#include "EngineMath.h"

class Entity;
class ParticleEmitterComponent;

namespace component
{
	class PoisonDebuff : public Component
	{
	public:
		PoisonDebuff(Entity* parent, int damagePerTick, int ticks, double tickDuration, float percentageSlow);
		~PoisonDebuff();

		void OnInitScene();
		void OnUnInitScene();

		void Reset(int damagePerTick, int ticks, double tickDuration, float percentageSlow);

		void Update(double dt);

	private:
		void tick();

		int m_Damage;
		int m_Ticks;
		double m_TickInterval;
		double m_TickTimer;
		float m_Slow;
	};


}




#endif // !POISON_DEBUFF_H
