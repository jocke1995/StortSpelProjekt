#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	this->position = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	this->rotationMat = DirectX::XMMatrixIdentity();
	this->scale = DirectX::XMFLOAT3(1.0, 1.0, 1.0);

	this->UpdateWorldMatrix();

	this->rotXMat = DirectX::XMMatrixIdentity();
	this->rotYMat = DirectX::XMMatrixIdentity();
	this->rotZMat = DirectX::XMMatrixIdentity();
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	this->position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
	this->position = pos;
}

void Transform::RotateX(float radians)
{
	this->rotXMat = DirectX::XMMatrixRotationX(radians);
}

void Transform::RotateY(float radians)
{
	this->rotYMat = DirectX::XMMatrixRotationY(radians);
}

void Transform::RotateZ(float radians)
{
	this->rotZMat = DirectX::XMMatrixRotationZ(radians);
}

void Transform::SetScale(float scale)
{
	this->scale = DirectX::XMFLOAT3(scale, scale, scale);
}

void Transform::SetScale(float x, float y, float z)
{
	this->scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::IncreaseScaleByPercent(float scale)
{
	this->scale.x += this->scale.x * scale;
	this->scale.y += this->scale.y * scale;
	this->scale.z += this->scale.z * scale;
}

void Transform::UpdateWorldMatrix()
{
	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
	DirectX::XMMATRIX sclMat = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	DirectX::XMMATRIX rotMat = this->rotationMat * this->rotXMat * this->rotYMat * this->rotZMat;

	this->worldMat = rotMat * sclMat * posMat;

	// Update transposed world matrix
	this->worldMatTransposed = DirectX::XMMatrixTranspose(this->worldMat);
}

DirectX::XMMATRIX* Transform::GetWorldMatrix()
{
	return &this->worldMat;
}

DirectX::XMMATRIX* Transform::GetWorldMatrixTransposed()
{
	return &this->worldMatTransposed;
}

DirectX::XMFLOAT3 Transform::GetPositionXMFLOAT3() const
{
	return this->position;
}

float3 Transform::GetPositionFloat3() const
{
	float3 pos = {};
	pos.x = this->position.x;
	pos.y = this->position.y;
	pos.z = this->position.z;
	return pos;
}
