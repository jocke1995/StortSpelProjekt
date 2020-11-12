#include "ProjectileComponent.h"
#include "Engine.h"
#include "HealthComponent.h"
#include "UpgradeComponents/UpgradeComponent.h"

#include "Events/Events.h"
#include "../ECS/Entity.h"

#include "../ECS/Components/Audio3DEmitterComponent.h"

component::ProjectileComponent::ProjectileComponent(Entity* parent, int damage, float ttl) : Component(parent)
{
	m_TimeToLive = ttl;
	m_CurrentDuration = 0.0f;
	m_Damage = damage;

	EventBus::GetInstance().Subscribe(this, &ProjectileComponent::hit);
}

component::ProjectileComponent::~ProjectileComponent()
{

}

void component::ProjectileComponent::Update(double dt)
{
	m_CurrentDuration += dt;

	if (m_CurrentDuration >= m_TimeToLive)
	{
		EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
	}
}

void component::ProjectileComponent::OnInitScene()
{
}

void component::ProjectileComponent::OnUnInitScene()
{
	EventBus::GetInstance().Unsubscribe(this, &ProjectileComponent::hit);
}

int component::ProjectileComponent::GetDamage() const
{
	return m_Damage;
}

int component::ProjectileComponent::GetTimeToLive() const
{
	return m_TimeToLive;
}

void component::ProjectileComponent::hit(Collision* event)
{
	// if we are the one that collided then make 
	// the other object lose health (if it has health)
	if (event->ent1 == m_pParent)
	{
		if (event->ent2->HasComponent<component::HealthComponent>())
		{
			event->ent2->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
			if (event->ent2->GetName().find("enemy") != std::string::npos && event->ent2->GetComponent<component::Audio3DEmitterComponent>())
			{
				event->ent2->GetComponent<component::Audio3DEmitterComponent>()->UpdateEmitter(L"Bruh");
				event->ent2->GetComponent<component::Audio3DEmitterComponent>()->Play(L"Bruh");
				EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
			}
		}
		// Call on upgrade on hit functions
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			m_pParent->GetComponent<component::UpgradeComponent>()->OnHit();
			m_pParent->GetComponent<component::UpgradeComponent>()->OnRangedHit();
		}
		EventBus::GetInstance().Unsubscribe(this, &ProjectileComponent::hit);
	}
	else if (event->ent2 == m_pParent)
	{
		if (event->ent1->HasComponent<component::HealthComponent>())
		{
			event->ent1->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
			if (event->ent1->GetName().find("enemy") != std::string::npos && event->ent1->GetComponent<component::Audio3DEmitterComponent>())
			{
				event->ent1->GetComponent<component::Audio3DEmitterComponent>()->UpdateEmitter(L"Bruh");
				event->ent1->GetComponent<component::Audio3DEmitterComponent>()->Play(L"Bruh");
				EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
			}
		}
		// Call on upgrade on hit functions
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			m_pParent->GetComponent<component::UpgradeComponent>()->OnHit();
			m_pParent->GetComponent<component::UpgradeComponent>()->OnRangedHit();
		}
		EventBus::GetInstance().Unsubscribe(this, &ProjectileComponent::hit);
	}
}
