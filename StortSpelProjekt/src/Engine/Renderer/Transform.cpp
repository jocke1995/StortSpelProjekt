#include "Transform.h"

Transform::Transform()
{
	this->position = XMFLOAT3(0.0, 0.0, 0.0);
	this->rotationMat = XMMatrixIdentity();
	this->scale = XMFLOAT3(1.0, 1.0, 1.0);

	this->UpdateWorldMatrix();

	this->rotXMat = XMMatrixIdentity();
	this->rotYMat = XMMatrixIdentity();
	this->rotZMat = XMMatrixIdentity();
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	this->position = XMFLOAT3(x, y, z);
}

void Transform::SetPosition(XMFLOAT3 pos)
{
	this->position = pos;
}

void Transform::RotateX(float radians)
{
	this->rotXMat = XMMatrixRotationX(radians);
}

void Transform::RotateY(float radians)
{
	this->rotYMat = XMMatrixRotationY(radians);
}

void Transform::RotateZ(float radians)
{
	this->rotZMat = XMMatrixRotationZ(radians);
}

void Transform::SetScale(float scale)
{
	this->scale = XMFLOAT3(scale, scale, scale);
}

void Transform::SetScale(float x, float y, float z)
{
	this->scale = XMFLOAT3(x, y, z);
}

void Transform::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
}

void Transform::IncreaseScaleByPercent(float scale)
{
	this->scale.x += this->scale.x * scale;
	this->scale.y += this->scale.y * scale;
	this->scale.z += this->scale.z * scale;
}

void Transform::UpdateWorldMatrix()
{
	XMMATRIX posMat = XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
	XMMATRIX sclMat = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	XMMATRIX rotMat = this->rotationMat * this->rotXMat * this->rotYMat * this->rotZMat;

	this->worldMat = rotMat * sclMat * posMat;
	//this->worldMat = XMMatrixIdentity();

	// Update transposed world matrix
	this->worldMatTransposed = DirectX::XMMatrixTranspose(this->worldMat);
}

XMMATRIX* Transform::GetWorldMatrix()
{
	return &this->worldMat;
}

XMMATRIX* Transform::GetWorldMatrixTransposed()
{
	return &this->worldMatTransposed;
}

XMFLOAT3 Transform::GetPositionXMFLOAT3() const
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
