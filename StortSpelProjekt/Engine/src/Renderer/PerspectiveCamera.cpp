#include "stdafx.h"
#include "PerspectiveCamera.h"

#include "TempInputClass.h"

// TEMPORARY CONSTRUCTOR
PerspectiveCamera::PerspectiveCamera(HINSTANCE hInstance, HWND hwnd)
	:BaseCamera()
{
	init();
	updateSpecific(0);

	m_TempHasInputObject = true;
	m_pTempInputClass = new TempInputClass();
	m_pTempInputClass->InitDirectInput(hInstance, hwnd);
}

PerspectiveCamera::PerspectiveCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, double fov, double aspectRatio, double zNear, double zFar)
	:BaseCamera(position, lookAt)
{
	init(fov, aspectRatio, zNear, zFar);
	updateSpecific(0);

	m_TempHasInputObject = false;
}

PerspectiveCamera::~PerspectiveCamera()
{
	if (m_TempHasInputObject == true)
	{
		delete m_pTempInputClass;
	}
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
	if (m_TempHasInputObject == true)
	{
		m_pTempInputClass->DetectInput(dt,
			&m_MoveForwardBackward,
			&m_MoveLeftRight,
			&m_MoveUpDown,
			&m_CamYaw,
			&m_CamPitch);

		updateCameraMovement();
	}
	

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

void PerspectiveCamera::updateCameraMovement()
{
	// Update the lookAt Vector depending on the mouse pitch/yaw.... WE DONT HAVE ROLL (hence '0' as the last parameter)
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_CamPitch, m_CamYaw, 0);
	DirectX::XMVECTOR defaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	m_AtVector = DirectX::XMVector3TransformCoord(defaultForward, camRotationMatrix);
	m_AtVector = DirectX::XMVector3Normalize(m_AtVector);

	// Update cameras forward,up and right vectors
	DirectX::XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = DirectX::XMMatrixRotationY(m_CamYaw);

	DirectX::XMVECTOR defaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	
	m_UpVector = XMVector3TransformCoord(m_UpVector, RotateYTempMatrix);
	m_AtVector = XMVector3TransformCoord(m_AtVector, RotateYTempMatrix);

	m_RightVector = DirectX::XMVector3Cross(m_AtVector, m_UpVector);
	
	m_EyeVector = DirectX::XMVectorAdd(m_EyeVector, DirectX::operator*(m_MoveLeftRight, m_RightVector));
	m_EyeVector = DirectX::XMVectorAdd(m_EyeVector, DirectX::operator*(m_AtVector,  m_MoveForwardBackward));
	m_EyeVector = DirectX::XMVectorAdd(m_EyeVector, DirectX::operator*(m_UpVector, m_MoveUpDown));

	m_MoveForwardBackward = 0.0f;
	m_MoveLeftRight = 0.0f;
	m_MoveUpDown = 0.0f;

	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_EyeVector, DirectX::XMVectorAdd(m_EyeVector, m_AtVector), m_UpVector);
}
