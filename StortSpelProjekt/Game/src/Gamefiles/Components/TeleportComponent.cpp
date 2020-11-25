#include "TeleportComponent.h"

#include "Events/EventBus.h"
#include "ECS/SceneManager.h"
#include "Player.h"
#include "Shop.h"

#include "ECS/Entity.h"

component::TeleportComponent::TeleportComponent(Entity* parent, Entity* player, std::string newSceneName)
	:Component(parent)
{
	m_NewSceneName = newSceneName;
	m_pPlayerInstance = player;
}

component::TeleportComponent::~TeleportComponent()
{
}

void component::TeleportComponent::OnInitScene()
{
	EventBus::GetInstance().Subscribe(this, &TeleportComponent::OnCollision);
}

void component::TeleportComponent::OnUnInitScene()
{
	EventBus::GetInstance().Unsubscribe(this, &TeleportComponent::OnCollision);
}

void component::TeleportComponent::OnCollision(Collision* collisionEvent)
{
	auto func = [&](const std::string newSceneName)
	{
		EventBus::GetInstance().Publish(&SceneChange(newSceneName));
		if (newSceneName == "ShopScene")
		{
			// Don't show old particles @(0,0,0) when tp:ing back
			m_pParent->GetComponent<component::ParticleEmitterComponent>()->Clear();
			Player::GetInstance().IsInShop(true);
		}
		else
		{
			Player::GetInstance().IsInShop(false);
			EventBus::GetInstance().Publish(&RoundStart());
		}
	};

	if (collisionEvent->ent1 == m_pPlayerInstance && collisionEvent->ent2 == m_pParent)
	{
		func(m_NewSceneName);
	}
	else if (collisionEvent->ent2 == m_pPlayerInstance && collisionEvent->ent1 == m_pParent)
	{
		func(m_NewSceneName);
	}
}
