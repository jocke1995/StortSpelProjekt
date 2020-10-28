#include "TeleportComponent.h"

#include "Events/EventBus.h"
#include "ECS/SceneManager.h"

component::TeleportComponent::TeleportComponent(Entity* parent, Entity* player, std::string newSceneName)
	:Component(parent)
{
	m_NewSceneName = newSceneName;
	m_pPlayerInstance = player;
	EventBus::GetInstance().Subscribe(this, &TeleportComponent::OnCollision);
}

component::TeleportComponent::~TeleportComponent()
{
	EventBus::GetInstance().Unsubscribe(this, &TeleportComponent::OnCollision);
}

void component::TeleportComponent::OnInitScene()
{
}

void component::TeleportComponent::OnUnInitScene()
{
}

void component::TeleportComponent::OnCollision(Collision* collisionEvent)
{
	if (collisionEvent->ent1 == m_pPlayerInstance && collisionEvent->ent2 == m_pParent)
	{
		EventBus::GetInstance().Publish(&SceneChange(m_NewSceneName));
	}
	else if (collisionEvent->ent2 == m_pPlayerInstance && collisionEvent->ent1 == m_pParent)
	{
		EventBus::GetInstance().Publish(&SceneChange(m_NewSceneName));
	}
}