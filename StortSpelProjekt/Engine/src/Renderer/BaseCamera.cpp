#include "stdafx.h"
#include "BaseCamera.h"

BaseCamera::BaseCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt)
{
	// Create View Matrix
	this->eyeVector = position;
	this->atVector = lookAt;
	this->upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	this->rightVector = DirectX::XMVector3Cross(this->atVector, this->upVector);

	this->viewMatrix = DirectX::XMMatrixLookAtLH(this->eyeVector, DirectX::XMVectorAdd(this->atVector, this->eyeVector), this->upVector);
}

BaseCamera::~BaseCamera()
{

}

void BaseCamera::Update(double dt)
{
	this->viewMatrix = DirectX::XMMatrixLookAtLH(this->eyeVector, DirectX::XMVectorAdd(this->atVector, this->eyeVector), this->upVector);

	// Inverse
	DirectX::XMVECTOR matInvDeter;	// Not used, but the function doesn't allow null params :)))
	this->viewMatrixInverse = XMMatrixInverse(&matInvDeter, this->viewMatrix);

	this->UpdateSpecific(dt);
}

void BaseCamera::SetPosition(float x, float y, float z)
{
	this->eyeVector = DirectX::XMVectorSet(x, y, z, 1.0);
}

void BaseCamera::SetLookAt(float x, float y, float z)
{
	this->atVector = DirectX::XMVectorSet(x, y, z, 0.0f);
}

DirectX::XMFLOAT3 BaseCamera::GetPosition() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->eyeVector);

	return DXfloat3;
}

float3 BaseCamera::GetPositionFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->eyeVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

DirectX::XMFLOAT3 BaseCamera::GetLookAt() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->atVector);

	return DXfloat3;
}

float3 BaseCamera::GetLookAtFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->atVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

const DirectX::XMMATRIX* BaseCamera::GetViewMatrix() const
{
	return &this->viewMatrix;
}

const DirectX::XMMATRIX* BaseCamera::GetViewMatrixInverse() const
{
	return &this->viewMatrixInverse;
}
