#include "stdafx.h"
#include "PerspectiveCamera.h"

#include "TempInputClass.h"

PerspectiveCamera::PerspectiveCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR direction, double fov, double aspectRatio, double zNear, double zFar)
	:BaseCamera(position, direction)
{
	init(fov, aspectRatio, zNear, zFar);
	updateSpecific(0);
}

PerspectiveCamera::~PerspectiveCamera()
{

}

void PerspectiveCamera::init(double fov, double aspectRatio, double zNear, double zFar)
{
	// Create Projection Matrix
	m_Fov = fov * DirectX::XM_PI / 180.0f;
	m_AspectRatio = aspectRatio;
	m_ZNear = zNear;
	m_ZFar = zFar;
	m_ProjMatrix = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_AspectRatio, m_ZNear, m_ZFar);
}

void PerspectiveCamera::updateSpecific(double dt)
{
	updateCameraMovement(dt);

	m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;
	m_ViewProjTranposedMatrix = DirectX::XMMatrixTranspose(m_ViewProjMatrix);
}

const DirectX::XMMATRIX* PerspectiveCamera::GetViewProjection() const
{
	return &m_ViewProjMatrix;
}

const DirectX::XMMATRIX* PerspectiveCamera::GetViewProjectionTranposed() const
{
	return &m_ViewProjTranposedMatrix;
}

void PerspectiveCamera::UpdateMovement(float x, float y, float z)
{
	m_MoveLeftRight += x;
	m_MoveForwardBackward += z;
	m_MoveUpDown += y;
}

void PerspectiveCamera::SetMovement(float x, float y, float z)
{
	m_MoveLeftRight = 0.0f;
	m_MoveForwardBackward = 0.0f;
	m_MoveUpDown = 0.0f;
}

void PerspectiveCamera::updateCameraMovement(double dt)
{
	m_RightVector = DirectX::XMVector3Cross(m_DirectionVector, m_UpVector);
	
	m_EyeVector = DirectX::XMVectorAdd(m_EyeVector, DirectX::operator*(m_RightVector, m_MoveLeftRight * 10 * dt));
	m_EyeVector = DirectX::XMVectorAdd(m_EyeVector, DirectX::operator*(m_DirectionVector,  m_MoveForwardBackward * 10 * dt));
	m_EyeVector = DirectX::XMVectorAdd(m_EyeVector, DirectX::operator*(m_UpVector, m_MoveUpDown * 10 * dt));

	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_EyeVector, DirectX::XMVectorAdd(m_EyeVector, m_DirectionVector), m_UpVector);
}
