#include "stdafx.h"
#include "PerspectiveCamera.h"

#include "TempInputClass.h"

// TEMPORARY CONSTRUCTOR
PerspectiveCamera::PerspectiveCamera(HINSTANCE hInstance, HWND hwnd)
	:BaseCamera()
{
	this->init();
	this->updateSpecific(0);

	this->m_TempHasInputObject = true;
	this->m_pTempInputClass = new TempInputClass();
	this->m_pTempInputClass->InitDirectInput(hInstance, hwnd);
}

PerspectiveCamera::PerspectiveCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, double fov, double aspectRatio, double zNear, double zFar)
	:BaseCamera(position, lookAt)
{
	this->init(fov, aspectRatio, zNear, zFar);
	this->updateSpecific(0);

	this->m_TempHasInputObject = false;
}

PerspectiveCamera::~PerspectiveCamera()
{
	if (m_TempHasInputObject == true)
	{
		delete this->m_pTempInputClass;
	}
}

void PerspectiveCamera::init(double fov, double aspectRatio, double zNear, double zFar)
{
	// Create Projection Matrix
	this->m_Fov = fov * DirectX::XM_PI / 180.0f;
	this->m_AspectRatio = aspectRatio;
	this->m_ZNear = zNear;
	this->m_ZFar = zFar;
	this->m_ProjMatrix = DirectX::XMMatrixPerspectiveFovLH(this->m_Fov, this->m_AspectRatio, this->m_ZNear, this->m_ZFar);
}

void PerspectiveCamera::updateSpecific(double dt)
{
	if (m_TempHasInputObject == true)
	{
		this->m_pTempInputClass->DetectInput(dt,
			&this->m_MoveForwardBackward,
			&this->m_MoveLeftRight,
			&this->m_MoveUpDown,
			&this->m_CamYaw,
			&this->m_CamPitch);

		this->updateCameraMovement();
	}
	

	this->m_ViewProjMatrix = this->m_ViewMatrix * this->m_ProjMatrix;
	this->m_ViewProjTranposedMatrix = DirectX::XMMatrixTranspose(this->m_ViewProjMatrix);
}

const DirectX::XMMATRIX* PerspectiveCamera::GetViewProjection() const
{
	return &this->m_ViewProjMatrix;
}

const DirectX::XMMATRIX* PerspectiveCamera::GetViewProjectionTranposed() const
{
	return &this->m_ViewProjTranposedMatrix;
}

void PerspectiveCamera::updateCameraMovement()
{
	// Update the lookAt Vector depending on the mouse pitch/yaw.... WE DONT HAVE ROLL (hence '0' as the last parameter)
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->m_CamPitch, this->m_CamYaw, 0);
	DirectX::XMVECTOR defaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	this->m_AtVector = DirectX::XMVector3TransformCoord(defaultForward, camRotationMatrix);
	this->m_AtVector = DirectX::XMVector3Normalize(this->m_AtVector);

	// Update cameras forward,up and right vectors
	DirectX::XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = DirectX::XMMatrixRotationY(this->m_CamYaw);

	DirectX::XMVECTOR defaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	
	this->m_UpVector = XMVector3TransformCoord(this->m_UpVector, RotateYTempMatrix);
	this->m_AtVector = XMVector3TransformCoord(this->m_AtVector, RotateYTempMatrix);

	this->m_RightVector = DirectX::XMVector3Cross(this->m_AtVector, this->m_UpVector);
	
	this->m_EyeVector = DirectX::XMVectorAdd(this->m_EyeVector, DirectX::operator*(this->m_MoveLeftRight, this->m_RightVector));
	this->m_EyeVector = DirectX::XMVectorAdd(this->m_EyeVector, DirectX::operator*(this->m_AtVector,  this->m_MoveForwardBackward));
	this->m_EyeVector = DirectX::XMVectorAdd(this->m_EyeVector, DirectX::operator*(this->m_UpVector, this->m_MoveUpDown));

	this->m_MoveForwardBackward = 0.0f;
	this->m_MoveLeftRight = 0.0f;
	this->m_MoveUpDown = 0.0f;

	this->m_ViewMatrix = DirectX::XMMatrixLookAtLH(this->m_EyeVector, DirectX::XMVectorAdd(this->m_EyeVector, this->m_AtVector), this->m_UpVector);
}
