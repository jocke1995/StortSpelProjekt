#include "AccelerationComponent.h"
#include "ECS/Entity.h"
#include "Renderer/Transform.h"
component::AccelerationComponent::AccelerationComponent(Entity* parent, double gravity): Component(parent)
{
	m_AccDir = { 0.0f,0.0f,0.0f };
	m_AccSpeed = 0;
	m_Gravity = gravity;
}

component::AccelerationComponent::~AccelerationComponent()
{
}

void component::AccelerationComponent::Update(double dt)
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

void component::AccelerationComponent::OnInitScene()
{
}

void component::AccelerationComponent::OnUnInitScene()
{
}

DirectX::XMFLOAT3 component::AccelerationComponent::GetDirection() const
{
	return m_AccDir;
}

float component::AccelerationComponent::GetAccSpeed() const
{
	return m_AccSpeed;
}

void component::AccelerationComponent::SetAccelerationDirection(DirectX::XMFLOAT3 dir)
{
	DirectX::XMStoreFloat3(&m_AccDir, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir)));
}

void component::AccelerationComponent::SetAccelerationDirection(float3 dir)
{
	DirectX::XMFLOAT3 direction(dir.x, dir.y, dir.z);
	DirectX::XMStoreFloat3(&m_AccDir, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)));
}

void component::AccelerationComponent::SetAccelerationDirection(float x, float y, float z)
{
	DirectX::XMFLOAT3 direction(x, y, z);
	DirectX::XMStoreFloat3(&m_AccDir, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)));
}

void component::AccelerationComponent::SetAccelerationSpeed(float speed)
{
	m_AccSpeed = speed;
}