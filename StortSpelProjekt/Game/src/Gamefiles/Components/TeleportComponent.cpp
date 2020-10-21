#include "TeleportComponent.h"

#include "Events/EventBus.h"

component::TeleportComponent::TeleportComponent(Entity* parent, Entity* player)
	:Component(parent)
{
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
		m_ChangeSceneThisFrame = true;
	}
	else if (collisionEvent->ent2 == m_pPlayerInstance && collisionEvent->ent1 == m_pParent)
	{
		m_ChangeSceneThisFrame = true;
	}
}

bool component::TeleportComponent::ChangeSceneThisFrame()
{
	bool changeScene = m_ChangeSceneThisFrame;
	if (changeScene == true)
	{
		m_ChangeSceneThisFrame = false;
	}

	return changeScene;
}
