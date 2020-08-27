#include "BaseCamera.h"

BaseCamera::BaseCamera(XMVECTOR position, XMVECTOR lookAt)
{
	// Create View Matrix
	this->eyeVector = position;
	this->atVector = lookAt;
	this->upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	this->rightVector = XMVector3Cross(this->atVector, this->upVector);

	this->viewMatrix = XMMatrixLookAtLH(this->eyeVector, this->atVector + this->eyeVector, this->upVector);
}

BaseCamera::~BaseCamera()
{

}

void BaseCamera::Update(double dt)
{
	this->viewMatrix = XMMatrixLookAtLH(this->eyeVector, this->atVector + this->eyeVector, this->upVector);

	// Inverse
	XMVECTOR matInvDeter;	// Not used, but the function doesn't allow null params :)))
	this->viewMatrixInverse = XMMatrixInverse(&matInvDeter, this->viewMatrix);

	this->UpdateSpecific(dt);
}

void BaseCamera::SetPosition(float x, float y, float z)
{
	this->eyeVector = XMVectorSet(x, y, z, 1.0);
}

void BaseCamera::SetLookAt(float x, float y, float z)
{
	this->atVector = XMVectorSet(x, y, z, 0.0f);
}

XMFLOAT3 BaseCamera::GetPosition() const
{
	XMFLOAT3 DXfloat3;
	XMStoreFloat3(&DXfloat3, this->eyeVector);

	return DXfloat3;
}

float3 BaseCamera::GetPositionFloat3() const
{
	XMFLOAT3 DXfloat3;
	XMStoreFloat3(&DXfloat3, this->eyeVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

XMFLOAT3 BaseCamera::GetLookAt() const
{
	XMFLOAT3 DXfloat3;
	XMStoreFloat3(&DXfloat3, this->atVector);

	return DXfloat3;
}

float3 BaseCamera::GetLookAtFloat3() const
{
	XMFLOAT3 DXfloat3;
	XMStoreFloat3(&DXfloat3, this->atVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

const XMMATRIX* BaseCamera::GetViewMatrix() const
{
	return &this->viewMatrix;
}

const XMMATRIX* BaseCamera::GetViewMatrixInverse() const
{
	return &this->viewMatrixInverse;
}
