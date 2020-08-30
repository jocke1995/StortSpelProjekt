#include "stdafx.h"
#include "PerspectiveCamera.h"

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

PerspectiveCamera::PerspectiveCamera(XMVECTOR position, XMVECTOR lookAt, double fov, double aspectRatio, double zNear, double zFar)
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
	this->fov = fov * XM_PI / 180.0f;
	this->aspectRatio = aspectRatio;
	this->zNear = zNear;
	this->zFar = zFar;
	this->projMatrix = XMMatrixPerspectiveFovLH(this->fov, this->aspectRatio, this->zNear, this->zFar);
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
	

	this->viewProjMatrix = this->viewMatrix * this->projMatrix;
	this->viewProjTranposedMatrix = DirectX::XMMatrixTranspose(this->viewProjMatrix);
}

const XMMATRIX* PerspectiveCamera::GetViewProjection() const
{
	return &this->viewProjMatrix;
}

const XMMATRIX* PerspectiveCamera::GetViewProjectionTranposed() const
{
	return &this->viewProjTranposedMatrix;
}

void PerspectiveCamera::UpdateCameraMovement()
{
	// Update the lookAt Vector depending on the mouse pitch/yaw.... WE DONT HAVE ROLL (hence '0' as the last parameter)
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->camPitch, this->camYaw, 0);
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	this->atVector = XMVector3TransformCoord(defaultForward, camRotationMatrix);
	this->atVector = XMVector3Normalize(this->atVector);

	// Update cameras forward,up and right vectors
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(this->camYaw);

	XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	
	this->upVector = XMVector3TransformCoord(this->upVector, RotateYTempMatrix);
	this->atVector = XMVector3TransformCoord(this->atVector, RotateYTempMatrix);

	this->rightVector = XMVector3Cross(this->atVector, this->upVector);

	this->eyeVector += this->rightVector * this->moveLeftRight;
	this->eyeVector += this->atVector * this->moveForwardBackward;
	this->eyeVector += this->upVector * this->moveUpDown;

	this->moveForwardBackward = 0.0f;
	this->moveLeftRight = 0.0f;
	this->moveUpDown = 0.0f;

	this->viewMatrix = XMMatrixLookAtLH(this->eyeVector, this->eyeVector + this->atVector, this->upVector);
}
