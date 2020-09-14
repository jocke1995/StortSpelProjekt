#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	m_Position = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	m_RotationMat = DirectX::XMMatrixIdentity();
	m_Scale = DirectX::XMFLOAT3(1.0, 1.0, 1.0);

	UpdateWorldMatrix();

	m_RotXMat = DirectX::XMMatrixIdentity();
	m_RotYMat = DirectX::XMMatrixIdentity();
	m_RotZMat = DirectX::XMMatrixIdentity();
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

void Transform::RotateX(float radians)
{
	m_RotXMat = DirectX::XMMatrixRotationX(radians);
}

void Transform::RotateY(float radians)
{
	m_RotYMat = DirectX::XMMatrixRotationY(radians);
}

void Transform::RotateZ(float radians)
{
	m_RotZMat = DirectX::XMMatrixRotationZ(radians);
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
	DirectX::XMMATRIX rotMat = m_RotationMat * m_RotXMat * m_RotYMat * m_RotZMat;

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
	return m_RotationMat * m_RotXMat * m_RotYMat * m_RotZMat;
}
