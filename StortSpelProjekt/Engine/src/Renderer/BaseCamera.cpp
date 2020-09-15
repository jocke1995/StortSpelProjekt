#include "stdafx.h"
#include "BaseCamera.h"

BaseCamera::BaseCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR direction)
{
	// Create View Matrix
	m_EyeVector = position;
	m_DirectionVector = direction;
	m_UpVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_RightVector = DirectX::XMVector3Cross(m_DirectionVector, m_UpVector);

	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_EyeVector, DirectX::XMVectorAdd(m_DirectionVector, m_EyeVector), m_UpVector);
}

BaseCamera::~BaseCamera()
{

}

void BaseCamera::Update(double dt)
{
	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_EyeVector, DirectX::XMVectorAdd(m_DirectionVector, m_EyeVector), m_UpVector);

	// Inverse
	DirectX::XMVECTOR matInvDeter;	// Not used, but the function doesn't allow null params :)))
	m_ViewMatrixInverse = XMMatrixInverse(&matInvDeter, m_ViewMatrix);

	updateSpecific(dt);
}

void BaseCamera::SetPosition(float x, float y, float z)
{
	m_EyeVector = DirectX::XMVectorSet(x, y, z, 1.0);
}

void BaseCamera::SetDirection(float x, float y, float z)
{
	m_DirectionVector = DirectX::XMVectorSet(x, y, z, 0.0f);
}

DirectX::XMFLOAT3 BaseCamera::GetPosition() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, m_EyeVector);

	return DXfloat3;
}

float3 BaseCamera::GetPositionFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, m_EyeVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

DirectX::XMFLOAT3 BaseCamera::GetDirection() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, m_DirectionVector);

	return DXfloat3;
}

float3 BaseCamera::GetDirectionFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, m_DirectionVector);

	float3 temp = {};
	temp.x = DXfloat3.x;
	temp.y = DXfloat3.y;
	temp.z = DXfloat3.z;

	return temp;
}

DirectX::XMFLOAT3 BaseCamera::GetRightVector() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, m_RightVector);

	return DXfloat3;
}

float3 BaseCamera::GetRightVectorFloat3() const
{
	DirectX::XMFLOAT3 DXfloat3;
	DirectX::XMStoreFloat3(&DXfloat3, m_RightVector);

	float3 toReturn = {};
	toReturn.x = DXfloat3.x;
	toReturn.y = DXfloat3.y;
	toReturn.z = DXfloat3.z;

	return toReturn;
}

const DirectX::XMMATRIX* BaseCamera::GetViewMatrix() const
{
	return &m_ViewMatrix;
}

const DirectX::XMMATRIX* BaseCamera::GetViewMatrixInverse() const
{
	return &m_ViewMatrixInverse;
}
