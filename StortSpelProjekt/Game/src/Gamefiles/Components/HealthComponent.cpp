#include "HealthComponent.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "ECS/Entity.h"

component::HealthComponent::HealthComponent(Entity* parent, int hp, float removalTime) : Component(parent)
{
	m_RemovalTimer = removalTime;
	m_Health = hp;
	// set max health to same as hp arg when created
	m_MaxHealth = m_Health;

	// temp so that we can print when health = 0
	EventBus::GetInstance().Subscribe(this, &HealthComponent::printDeath);
	
}

component::HealthComponent::~HealthComponent()
{

}

void component::HealthComponent::Update(double dt)
{
	m_DeathDuration += static_cast<double>(m_Dead * dt);
}

void component::HealthComponent::OnInitScene()
{
}

void component::HealthComponent::OnUnInitScene()
{
}

void component::HealthComponent::SetHealth(int hp)
{
	m_Health = hp;
	if (m_Health <= 0 && m_Dead == false)
	{
		if (m_RemovalTimer <= m_DeathDuration)
		{
			EventBus::GetInstance().Publish(&Death(m_pParent));
			m_Dead = true;
		}
	}
}

void component::HealthComponent::ChangeHealth(int hpChange)
{
	m_Health += hpChange * static_cast<float>(m_Health > 0);
	if (m_Health <= 0 && m_Dead == false)
	{
		EventBus::GetInstance().Publish(&Death(m_pParent));
		m_Dead = true;
	}

	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}

}

int component::HealthComponent::GetHealth() const
{
	return m_Health;
}

int component::HealthComponent::GetMaxHealth() const
{
	return m_MaxHealth;
}

void component::HealthComponent::SetMaxHealth(int newHealth)
{
	m_MaxHealth = newHealth;
}

void component::HealthComponent::ChangeMaxHealth(int hpChange)
{
	m_MaxHealth += hpChange;
}


void component::HealthComponent::printDeath(Death* event)
{
	// TODO: When more of the game is inplace make an improved version of this function
	if (event->ent == m_pParent)
	{
		Log::Print("%s died!\n", event->ent->GetName().c_str());
		component::CollisionComponent* cc = m_pParent->GetComponent<component::CollisionComponent>();
		cc->SetVelVector(0.0, 0.0, 0.0);
		cc->SetAngularVelocity(0.0, 0.0, 0.0);
		cc->SetAngularFactor({ 1.0, 1.0, 1.0 });
		cc->SetFriction(1000.0);
	}
}
