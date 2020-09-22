#include "AccelrationComponent.h"
#include "ECS/Entity.h"
#include "Renderer/Transform.h"
component::AccelrationComponent::AccelrationComponent(Entity* parent, double gravity): Component(parent)
{
	m_AccDir = { 0.0f,0.0f,0.0f };
	m_AccSpeed = 0;
	m_Gravity = gravity;
}

component::AccelrationComponent::~AccelrationComponent()
{
}

void component::AccelrationComponent::Update(double dt)
{
	Transform* trans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
	trans->UpdateMovement((m_AccDir.x * m_AccSpeed)*dt, (m_AccDir.y * m_AccSpeed - m_Gravity) * dt, (m_AccDir.z * m_AccSpeed)*dt);

	// This if statement will later on be replaced by collision with floor.
	if (trans->GetPositionFloat3().y <= 1)
	{
		trans->SetMovement({ trans->GetMovement().x, 0, trans->GetMovement().z });
		trans->SetPosition(trans->GetPositionFloat3().x, 1, trans->GetPositionFloat3().z);
	}
}

DirectX::XMFLOAT3 component::AccelrationComponent::GetDirection() const
{
	return m_AccDir;
}

float component::AccelrationComponent::GetAccSpeed() const
{
	return m_AccSpeed;
}

void component::AccelrationComponent::SetAccelrationDirection(DirectX::XMFLOAT3 dir)
{
	DirectX::XMStoreFloat3(&m_AccDir, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir)));
}

void component::AccelrationComponent::SetAccelrationSpeed(float speed)
{
	m_AccSpeed = speed;
}
