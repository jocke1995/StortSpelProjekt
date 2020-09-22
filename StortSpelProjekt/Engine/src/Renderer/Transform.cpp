#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	m_Position = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	m_RotationMat = DirectX::XMMatrixIdentity();
	m_Scale = DirectX::XMFLOAT3(1.0, 1.0, 1.0);
	m_Movement = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	UpdateWorldMatrix();

	m_RotXMat = DirectX::XMMatrixIdentity();
	m_RotYMat = DirectX::XMMatrixIdentity();
	m_RotZMat = DirectX::XMMatrixIdentity();
	m_Velocity = 10;
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	m_Position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
	m_Position = pos;
}

void Transform::SetMovement(float x, float y, float z)
{
	m_Movement = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetMovement(DirectX::XMFLOAT3 mov)
{
	m_Movement = mov;
}

void Transform::UpdateMovement(float x, float y, float z)
{
	m_Movement = DirectX::XMFLOAT3(m_Movement.x + x, m_Movement.y + y, m_Movement.z + z);
}

void Transform::Move(float dt)
{
	DirectX::XMFLOAT3 normalizedMovement;
	// Normalize movement
	DirectX::XMVECTOR movementVector = DirectX::XMLoadFloat3(&m_Movement);
	movementVector = DirectX::XMVector3Normalize(movementVector);
	DirectX::XMStoreFloat3(&normalizedMovement, movementVector);
	float moveX = m_Position.x + (normalizedMovement.x * m_Velocity * dt);
	float moveY = m_Position.y + (normalizedMovement.y * m_Velocity * dt);
	float moveZ = m_Position.z + (normalizedMovement.z * m_Velocity * dt);

	m_Position = DirectX::XMFLOAT3(moveX, moveY, moveZ);
}

void Transform::MoveRender(float dt)
{
	DirectX::XMFLOAT3 normalizedMovement;
	// Normalize movement
	DirectX::XMVECTOR movementVector = DirectX::XMLoadFloat3(&m_Movement);
	movementVector = DirectX::XMVector3Normalize(movementVector);
	DirectX::XMStoreFloat3(&normalizedMovement, movementVector);
	float moveX = m_RenderPosition.x + (normalizedMovement.x * m_Velocity * dt);
	float moveY = m_RenderPosition.y + (normalizedMovement.y * m_Velocity * dt);
	float moveZ = m_RenderPosition.z + (normalizedMovement.z * m_Velocity * dt);

	m_Position = DirectX::XMFLOAT3(moveX, moveY, moveZ);
}

void Transform::SetRotationX(float radians)
{
	m_RotXMat = DirectX::XMMatrixRotationX(radians);
	m_RotationMat = m_RotXMat * m_RotYMat * m_RotZMat;
}

void Transform::SetRotationY(float radians)
{
	m_RotYMat = DirectX::XMMatrixRotationY(radians);
	m_RotationMat = m_RotXMat * m_RotYMat * m_RotZMat;
}

void Transform::SetRotationZ(float radians)
{
	m_RotZMat = DirectX::XMMatrixRotationZ(radians);
	m_RotationMat = m_RotXMat * m_RotYMat * m_RotZMat;
}

void Transform::SetScale(float scale)
{
	m_Scale = DirectX::XMFLOAT3(scale, scale, scale);
}

void Transform::SetScale(float x, float y, float z)
{
	m_Scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::IncreaseScaleByPercent(float scale)
{
	m_Scale.x += m_Scale.x * scale;
	m_Scale.y += m_Scale.y * scale;
	m_Scale.z += m_Scale.z * scale;
}

void Transform::UpdateWorldMatrix()
{
	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	DirectX::XMMATRIX sclMat = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	DirectX::XMMATRIX rotMat = m_RotationMat;

	m_WorldMat = rotMat * sclMat * posMat;

	// Update transposed world matrix
	m_WorldMatTransposed = DirectX::XMMatrixTranspose(m_WorldMat);
}

DirectX::XMMATRIX* Transform::GetWorldMatrix()
{
	return &m_WorldMat;
}

DirectX::XMMATRIX* Transform::GetWorldMatrixTransposed()
{
	return &m_WorldMatTransposed;
}

DirectX::XMFLOAT3 Transform::GetPositionXMFLOAT3() const
{
	return m_Position;
}

float3 Transform::GetPositionFloat3() const
{
	float3 pos = {};
	pos.x = m_Position.x;
	pos.y = m_Position.y;
	pos.z = m_Position.z;
	return pos;
}

DirectX::XMFLOAT3 Transform::GetScale() const
{
	return m_Scale;
}

DirectX::XMMATRIX Transform::GetRotMatrix() const
{
	return m_RotationMat;
}

DirectX::XMFLOAT3 Transform::GetMovement() const
{
	return m_Movement;
}

float Transform::GetVelocity() const
{
	return m_Velocity;
}

void Transform::SetVelocity(float vel)
{
	m_Velocity = vel;
}

void Transform::SetActualMovement(float x, float y, float z)
{
	m_Movement = DirectX::XMFLOAT3(x, y, z);
	m_Velocity = sqrt(x * x + y * y + z * z);
}

void Transform::SetActualMovement(DirectX::XMFLOAT3 mov)
{
	m_Movement = mov;
	m_Velocity = sqrt(mov.x * mov.x + mov.y * mov.y + mov.z * mov.z);
}
