#include "stdafx.h"
#include "BaseCamera.h"

BaseCamera::BaseCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt)
{
	// Create View Matrix
	this->m_EyeVector = position;
	this->m_AtVector = lookAt;
	this->m_UpVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	this->m_RightVector = DirectX::XMVector3Cross(this->m_AtVector, this->m_UpVector);

	this->m_ViewMatrix = DirectX::XMMatrixLookAtLH(this->m_EyeVector, DirectX::XMVectorAdd(this->m_AtVector, this->m_EyeVector), this->m_UpVector);
}

BaseCamera::~BaseCamera()
{

}

void BaseCamera::Update(double dt)
{
	this->m_ViewMatrix = DirectX::XMMatrixLookAtLH(this->m_EyeVector, DirectX::XMVectorAdd(this->m_AtVector, this->m_EyeVector), this->m_UpVector);

	// Inverse
	DirectX::XMVECTOR matInvDeter;	// Not used, but the function doesn't allow null params :)))
	this->m_ViewMatrixInverse = XMMatrixInverse(&matInvDeter, this->m_ViewMatrix);

	this->updateSpecific(dt);
}

void BaseCamera::SetPosition(float x, float y, float z)
{
	this->m_EyeVector = DirectX::XMVectorSet(x, y, z, 1.0);
}

void BaseCamera::SetLookAt(float x, float y, float z)
{
	this->m_AtVector = DirectX::XMVectorSet(x, y, z, 0.0f);
}

DirectX::XMFLOAT3 BaseCamera::GetPosition() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->m_EyeVector);

	return DXfloat3;
}

float3 BaseCamera::GetPositionFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->m_EyeVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

DirectX::XMFLOAT3 BaseCamera::GetLookAt() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->m_AtVector);

	return DXfloat3;
}

float3 BaseCamera::GetLookAtFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, this->m_AtVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

const DirectX::XMMATRIX* BaseCamera::GetViewMatrix() const
{
	return &this->m_ViewMatrix;
}

const DirectX::XMMATRIX* BaseCamera::GetViewMatrixInverse() const
{
	return &this->m_ViewMatrixInverse;
}
