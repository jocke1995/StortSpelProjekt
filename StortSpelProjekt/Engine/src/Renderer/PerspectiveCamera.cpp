#include "stdafx.h"
#include "PerspectiveCamera.h"

#include "TempInputClass.h"

// TEMPORARY CONSTRUCTOR
PerspectiveCamera::PerspectiveCamera(HINSTANCE hInstance, HWND hwnd)
	:BaseCamera()
{
	this->Init();
	this->UpdateSpecific(0);

	this->tempHasInputObject = true;
	this->tempInputClass = new TempInputClass();
	this->tempInputClass->InitDirectInput(hInstance, hwnd);
}

PerspectiveCamera::PerspectiveCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, double fov, double aspectRatio, double zNear, double zFar)
	:BaseCamera(position, lookAt)
{
	this->Init(fov, aspectRatio, zNear, zFar);
	this->UpdateSpecific(0);

	this->tempHasInputObject = false;
}

PerspectiveCamera::~PerspectiveCamera()
{
	if (tempHasInputObject == true)
	{
		delete this->tempInputClass;
	}
}

void PerspectiveCamera::Init(double fov, double aspectRatio, double zNear, double zFar)
{
	// Create Projection Matrix
	this->fov = fov * DirectX::XM_PI / 180.0f;
	this->aspectRatio = aspectRatio;
	this->zNear = zNear;
	this->zFar = zFar;
	this->projMatrix = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->aspectRatio, this->zNear, this->zFar);
}

void PerspectiveCamera::UpdateSpecific(double dt)
{
	if (tempHasInputObject == true)
	{
		this->tempInputClass->DetectInput(dt,
			&this->moveForwardBackward,
			&this->moveLeftRight,
			&this->moveUpDown,
			&this->camYaw,
			&this->camPitch);

		this->UpdateCameraMovement();
	}
	

	this->viewProjMatrix = this->m_ViewMatrix * this->projMatrix;
	this->viewProjTranposedMatrix = DirectX::XMMatrixTranspose(this->viewProjMatrix);
}

const DirectX::XMMATRIX* PerspectiveCamera::GetViewProjection() const
{
	return &this->viewProjMatrix;
}

const DirectX::XMMATRIX* PerspectiveCamera::GetViewProjectionTranposed() const
{
	return &this->viewProjTranposedMatrix;
}

void PerspectiveCamera::UpdateCameraMovement()
{
	// Update the lookAt Vector depending on the mouse pitch/yaw.... WE DONT HAVE ROLL (hence '0' as the last parameter)
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->camPitch, this->camYaw, 0);
	DirectX::XMVECTOR defaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	this->m_AtVector = DirectX::XMVector3TransformCoord(defaultForward, camRotationMatrix);
	this->m_AtVector = DirectX::XMVector3Normalize(this->m_AtVector);

	// Update cameras forward,up and right vectors
	DirectX::XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = DirectX::XMMatrixRotationY(this->camYaw);

	DirectX::XMVECTOR defaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	
	this->m_UpVector = XMVector3TransformCoord(this->m_UpVector, RotateYTempMatrix);
	this->m_AtVector = XMVector3TransformCoord(this->m_AtVector, RotateYTempMatrix);

	this->m_RightVector = DirectX::XMVector3Cross(this->m_AtVector, this->m_UpVector);
	
	this->m_EyeVector = DirectX::XMVectorAdd(this->m_EyeVector, DirectX::operator*(this->moveLeftRight, this->m_RightVector));
	this->m_EyeVector = DirectX::XMVectorAdd(this->m_EyeVector, DirectX::operator*(this->m_AtVector,  this->moveForwardBackward));
	this->m_EyeVector = DirectX::XMVectorAdd(this->m_EyeVector, DirectX::operator*(this->m_UpVector, this->moveUpDown));

	this->moveForwardBackward = 0.0f;
	this->moveLeftRight = 0.0f;
	this->moveUpDown = 0.0f;

	this->m_ViewMatrix = DirectX::XMMatrixLookAtLH(this->m_EyeVector, DirectX::XMVectorAdd(this->m_EyeVector, this->m_AtVector), this->m_UpVector);
}
