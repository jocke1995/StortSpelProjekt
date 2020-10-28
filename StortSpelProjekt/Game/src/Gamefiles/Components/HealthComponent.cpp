#include "HealthComponent.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "ECS/Entity.h"
#include "UpgradeComponents/UpgradeComponent.h"

component::HealthComponent::HealthComponent(Entity* parent, int hp, float removalTime) : Component(parent)
{
	m_FlatDamageReduction = 0;
	m_ProcentileDamageReduction = 1.0f;
	m_RemovalTimer = removalTime;
	m_Health = hp;
	// set max health to same as hp arg when created
	m_MaxHealth = m_Health;

	m_FlatDamageReduction = 0;
	m_ProcentileDamageReduction = 1.0;
}

component::HealthComponent::~HealthComponent()
{

}

void component::HealthComponent::Update(double dt)
{
	m_DeathDuration += static_cast<double>(m_Dead * dt);
	if (m_RemovalTimer <= m_DeathDuration && m_Dead)
	{
		if (m_pParent->GetName() != "player")
		{
			EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
		}
	}
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
		m_Dead = true;
		component::CollisionComponent* comp = m_pParent->GetComponent<component::CollisionComponent>();
		if (comp)
		{
			comp->SetAngularFactor({ 1,1,1 });
		}
		EventBus::GetInstance().Publish(&Death(m_pParent));
	}
}

void component::HealthComponent::ChangeHealth(int hpChange)
{
	m_Health += hpChange * static_cast<float>(m_Health > 0);
	if (m_Health <= 0 && m_Dead == false)
	{
		m_Dead = true;
		component::CollisionComponent* comp = m_pParent->GetComponent<component::CollisionComponent>();
		if (comp)
		{
			comp->SetAngularFactor({ 1,1,1 });
		}
		EventBus::GetInstance().Publish(&Death(m_pParent));
	}

	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}

}

void component::HealthComponent::TakeDamage(int damage)
{
	// Call on upgrade on damage functions
	if (m_pParent->HasComponent<component::UpgradeComponent>())
	{
		m_pParent->GetComponent<component::UpgradeComponent>()->OnDamage();
	}

	ChangeHealth((damage - m_FlatDamageReduction) * m_ProcentileDamageReduction); //Flat Damage gets applied first followed by multaplicative damage
	//Damage reduction stat is reset to allow upgrade to change again
	m_FlatDamageReduction = 0.0;
	m_ProcentileDamageReduction = 1.0;
}

void component::HealthComponent::ChangeFlatDamageReduction(int flatDamageReduction)
{
	m_FlatDamageReduction += flatDamageReduction;
}

void component::HealthComponent::ChangeProcentileDamageReduction(float procentileDamageReduction)
{
	m_ProcentileDamageReduction *= procentileDamageReduction;
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

void component::HealthComponent::Reset()
{
	m_Health = m_MaxHealth;
	m_Dead = false;
}