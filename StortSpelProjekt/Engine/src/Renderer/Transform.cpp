#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	this->m_Position = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	this->m_RotationMat = DirectX::XMMatrixIdentity();
	this->m_Scale = DirectX::XMFLOAT3(1.0, 1.0, 1.0);

	this->UpdateWorldMatrix();

	this->m_RotXMat = DirectX::XMMatrixIdentity();
	this->m_RotYMat = DirectX::XMMatrixIdentity();
	this->m_RotZMat = DirectX::XMMatrixIdentity();
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	this->m_Position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
	this->m_Position = pos;
}

void Transform::RotateX(float radians)
{
	this->m_RotXMat = DirectX::XMMatrixRotationX(radians);
}

void Transform::RotateY(float radians)
{
	this->m_RotYMat = DirectX::XMMatrixRotationY(radians);
}

void Transform::RotateZ(float radians)
{
	this->m_RotZMat = DirectX::XMMatrixRotationZ(radians);
}

void Transform::SetScale(float scale)
{
	this->m_Scale = DirectX::XMFLOAT3(scale, scale, scale);
}

void Transform::SetScale(float x, float y, float z)
{
	this->m_Scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::IncreaseScaleByPercent(float scale)
{
	this->m_Scale.x += this->m_Scale.x * scale;
	this->m_Scale.y += this->m_Scale.y * scale;
	this->m_Scale.z += this->m_Scale.z * scale;
}

void Transform::UpdateWorldMatrix()
{
	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslation(this->m_Position.x, this->m_Position.y, this->m_Position.z);
	DirectX::XMMATRIX sclMat = DirectX::XMMatrixScaling(this->m_Scale.x, this->m_Scale.y, this->m_Scale.z);
	DirectX::XMMATRIX rotMat = this->m_RotationMat * this->m_RotXMat * this->m_RotYMat * this->m_RotZMat;

	this->m_WorldMat = rotMat * sclMat * posMat;

	// Update transposed world matrix
	this->m_WorldMatTransposed = DirectX::XMMatrixTranspose(this->m_WorldMat);
}

DirectX::XMMATRIX* Transform::GetWorldMatrix()
{
	return &this->m_WorldMat;
}

DirectX::XMMATRIX* Transform::GetWorldMatrixTransposed()
{
	return &this->m_WorldMatTransposed;
}

DirectX::XMFLOAT3 Transform::GetPositionXMFLOAT3() const
{
	return this->m_Position;
}

float3 Transform::GetPositionFloat3() const
{
	float3 pos = {};
	pos.x = this->m_Position.x;
	pos.y = this->m_Position.y;
	pos.z = this->m_Position.z;
	return pos;
}
