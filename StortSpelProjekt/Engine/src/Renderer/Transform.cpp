#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	m_Position = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	m_RenderPosition = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	m_RotationMat = DirectX::XMMatrixIdentity();
	m_Scale = DirectX::XMFLOAT3(1.0, 1.0, 1.0);
	m_Movement = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	UpdateWorldMatrix();

	m_RotXMat = DirectX::XMMatrixIdentity();
	m_RotYMat = DirectX::XMMatrixIdentity();
	m_RotZMat = DirectX::XMMatrixIdentity();
	m_Velocity = BASE_VEL;
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	m_Position = DirectX::XMFLOAT3(x, y, z);
	m_RenderPosition = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
	m_Position = pos;
	m_RenderPosition = pos;
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
	float moveX = m_Position.x + (m_Movement.x * m_Velocity * dt);
	float moveY = m_Position.y + (m_Movement.y * m_Velocity * dt);
	float moveZ = m_Position.z + (m_Movement.z * m_Velocity * dt);

	m_Position = DirectX::XMFLOAT3(moveX, moveY, moveZ);
	m_RenderPosition = m_Position;
}

void Transform::MoveRender(float dt)
{
	float moveX = m_RenderPosition.x + (m_Movement.x * m_Velocity * dt);
	float moveY = m_RenderPosition.y + (m_Movement.y * m_Velocity * dt);
	float moveZ = m_RenderPosition.z + (m_Movement.z * m_Velocity * dt);

	m_RenderPosition = DirectX::XMFLOAT3(moveX, moveY, moveZ);
}

void Transform::NormalizedMove(float dt)
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
	m_RenderPosition = m_Position;
}

void Transform::NormalizedMoveRender(float dt)
{
	DirectX::XMFLOAT3 normalizedMovement;
	// Normalize movement
	DirectX::XMVECTOR movementVector = DirectX::XMLoadFloat3(&m_Movement);
	movementVector = DirectX::XMVector3Normalize(movementVector);
	DirectX::XMStoreFloat3(&normalizedMovement, movementVector);
	float moveX = m_RenderPosition.x + (normalizedMovement.x * m_Velocity * dt);
	float moveY = m_RenderPosition.y + (normalizedMovement.y * m_Velocity * dt);
	float moveZ = m_RenderPosition.z + (normalizedMovement.z * m_Velocity * dt);

	m_RenderPosition = DirectX::XMFLOAT3(moveX, moveY, moveZ);
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
	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslation(m_RenderPosition.x, m_RenderPosition.y, m_RenderPosition.z);
	DirectX::XMMATRIX sclMat = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	DirectX::XMMATRIX rotMat = m_RotationMat;

	m_WorldMat = sclMat * rotMat * posMat;

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

DirectX::XMFLOAT3 Transform::GetRenderPositionXMFLOAT3() const
{
	return m_RenderPosition;
}

float3 Transform::GetRenderPositionFloat3() const
{
	float3 pos = {};
	pos.x = m_RenderPosition.x;
	pos.y = m_RenderPosition.y;
	pos.z = m_RenderPosition.z;
	return pos;
}

DirectX::XMFLOAT3 Transform::GetScale() const
{
	return m_Scale;
}

float4 Transform::GetRotation()
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationMatrix(m_RotationMat);
	return { quat.m128_f32[0], quat.m128_f32[1], quat.m128_f32[2], quat.m128_f32[3] };
}

DirectX::XMMATRIX Transform::GetRotMatrix() const
{
	return m_RotationMat;
}

DirectX::XMFLOAT3 Transform::GetMovement() const
{
	return m_Movement;
}

DirectX::XMFLOAT3 Transform::GetForwardXMFLOAT3() const
{
	DirectX::XMFLOAT3 forward;
	DirectX::XMStoreFloat3(&forward, m_RotationMat.r[2]);

	return forward;
}

float3 Transform::GetForwardFloat3() const
{
	DirectX::XMFLOAT3 forward;
	DirectX::XMStoreFloat3(&forward, m_RotationMat.r[2]);

	return { forward.x, forward.y, forward.z };
}

DirectX::XMFLOAT3 Transform::GetRightXMFLOAT3() const
{
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, m_RotationMat.r[0]);

	return right;
}
float3 Transform::GetRightFloat3() const
{
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, m_RotationMat.r[0]);

	return { right.x, right.y, right.z };
}
DirectX::XMFLOAT3 Transform::GetUpXMFLOAT3() const
{
	DirectX::XMFLOAT3 up;
	DirectX::XMStoreFloat3(&up, m_RotationMat.r[1]);

	return up;
}
float3 Transform::GetUpFloat3() const
{
	DirectX::XMFLOAT3 up;
	DirectX::XMStoreFloat3(&up, m_RotationMat.r[1]);

	return { up.x, up.y, up.z };
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

void Transform::UpdateActualMovement(float x, float y, float z)
{
	m_Movement = DirectX::XMFLOAT3(m_Movement.x + x, m_Movement.y + y, m_Movement.z + z);
	DirectX::XMStoreFloat(&m_Velocity,DirectX::XMVector3Length(DirectX::XMLoadFloat3(&m_Movement)));
}
