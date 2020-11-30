#include "SpeedModifierComponent.h"
#include "ECS/Entity.h"
#include "Renderer/Transform.h"

component::SpeedModifier::SpeedModifier(Entity* parent) : Component(parent)
{
	m_BaseVelocity = parent->GetComponent<component::TransformComponent>()->GetTransform()->GetVelocity();
}

component::SpeedModifier::~SpeedModifier()
{
}

void component::SpeedModifier::Update(double dt)
{
	if (m_TemporarySpeedMod < 1.0f)
	{
		m_TemporarySpeedMod += 0.1 * dt; //Recover 10% movementspeed every second
		if (m_TemporarySpeedMod > 1.0f)
		{
			m_TemporarySpeedMod = 1.0f;
		}
		m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->SetVelocity(m_BaseVelocity * m_TemporarySpeedMod);
	}
	else if (m_TemporarySpeedMod > 1.0f)
	{
		m_TemporarySpeedMod -= 0.1 * dt; //Lose 10% movementspeed every second
		if (m_TemporarySpeedMod < 1.0f)
		{
			m_TemporarySpeedMod = 1.0f;
		}
		m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->SetVelocity(m_BaseVelocity * m_TemporarySpeedMod);
	}
}

void component::SpeedModifier::SetTemporayModifier(float tempMod)
{
	m_TemporarySpeedMod = tempMod;
	m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->SetVelocity(m_BaseVelocity * m_TemporarySpeedMod);
}

void component::SpeedModifier::UpdateBaseVelocity()
{
	m_BaseVelocity = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetVelocity();
}

void component::SpeedModifier::OnInitScene()
{
}

void component::SpeedModifier::OnUnInitScene()
{
}
