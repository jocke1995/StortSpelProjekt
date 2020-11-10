#include "HealthComponent.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "ECS/Entity.h"
#include "UpgradeComponents/UpgradeComponent.h"
#include "Player.h"
#include "PoisonDebuff.h"

component::PoisonDebuff::PoisonDebuff(Entity* parent, int damagePerTick, int ticks, double tickDuration, float percentageSlow) : Component(parent)
{
	Reset(damagePerTick, ticks, tickDuration, percentageSlow);
}

component::PoisonDebuff::~PoisonDebuff()
{
}

void component::PoisonDebuff::Reset(int damagePerTick, int ticks, double tickDuration, float percentageSlow)
{
	m_Damage = damagePerTick;
	m_Ticks = ticks;
	m_TickDuration = tickDuration;
	Transform* transform = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
	m_Slow = transform->GetVelocity() * percentageSlow;
	transform->SetVelocity(transform->GetVelocity() - m_Slow);
	m_TickTimer = 0.0;
}

void component::PoisonDebuff::Update(double dt)
{
	if (m_Ticks > 0)
	{
		m_TickTimer += dt;
		if (m_TickTimer >= m_TickDuration)
		{
			m_TickTimer -= m_TickDuration;
			m_Ticks--;
			m_pParent->GetComponent<component::HealthComponent>()->TakeDamage(m_Damage);
			if (m_Ticks == 0)
			{
				Transform* transform = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
				transform->SetVelocity(transform->GetVelocity() + m_Slow);
			}
		}
	}
}
