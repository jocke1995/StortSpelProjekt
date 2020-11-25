#include "stdafx.h"
#include "TemporaryLifeComponent.h"
#include "../ECS/Entity.h"
#include "../Events/EventBus.h"

component::TemporaryLifeComponent::TemporaryLifeComponent(Entity* parent, double duration) : Component(parent)
{
	m_Duration = duration;
}

component::TemporaryLifeComponent::~TemporaryLifeComponent()
{
}

void component::TemporaryLifeComponent::RenderUpdate(double dt)
{
	m_Duration -= dt;
	if (m_Duration <= 0)
	{
		EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
	}
}

void component::TemporaryLifeComponent::OnInitScene()
{
}

void component::TemporaryLifeComponent::OnUnInitScene()
{
}
