#include "stdafx.h"
#include "CameraComponent.h"

namespace component
{
	// Temp Constructor to handle input
	CameraComponent::CameraComponent(Entity* parent, HINSTANCE hInstance, HWND hwnd, bool primary)
		:Component(parent)
	{
		this->camType = CAMERA_TYPE::PERSPECTIVE;
		this->primaryCamera = primary;
		this->camera = new PerspectiveCamera(hInstance, hwnd);
	}

	// Default Settings
	CameraComponent::CameraComponent(Entity* parent, CAMERA_TYPE camType, bool primary)
		:Component(parent)
	{
		this->primaryCamera = primary;

		switch (this->camType)
		{
		case CAMERA_TYPE::PERSPECTIVE:
			this->camera = this->CreatePerspective();
			break;
		case CAMERA_TYPE::ORTHOGRAPHIC:
			this->camera = this->CreateOrthographic();
			break;
		}
	}

	// Perspective Constructor
	CameraComponent::CameraComponent(Entity* parent, bool primary, XMVECTOR position, XMVECTOR lookAt, double fov, double aspectRatio, double zNear, double zFar)
		: Component(parent)
	{
		this->primaryCamera = primary;
		this->camera = this->CreatePerspective(
			position, lookAt,
			fov, aspectRatio,
			zNear, zFar);
	}

	// Orthographic Constructor
	CameraComponent::CameraComponent(Entity* parent, bool primary, XMVECTOR position, XMVECTOR lookAt, float left, float right, float bot, float top, float nearZ, float farZ)
		: Component(parent)
	{
		this->primaryCamera = primary;
		this->camera = this->CreateOrthographic(
			position, lookAt,
			left, right, bot, top,
			nearZ, farZ);
	}

	CameraComponent::~CameraComponent()
	{
		delete this->camera;
	}

	BaseCamera* CameraComponent::GetCamera() const
	{
		return this->camera;
	}

	bool CameraComponent::IsPrimary() const
	{
		return this->primaryCamera;
	}

	void CameraComponent::Update(double dt)
	{
		this->camera->Update(dt);
	}

	BaseCamera* CameraComponent::CreatePerspective(XMVECTOR position, XMVECTOR lookAt, double fov, double aspectRatio, double nearZ, double farZ)
	{
		this->camType = CAMERA_TYPE::PERSPECTIVE;
		return new PerspectiveCamera(
			position, lookAt,
			fov, aspectRatio,
			nearZ, farZ);
	}

	BaseCamera* CameraComponent::CreateOrthographic(XMVECTOR position, XMVECTOR lookAt, float left, float right, float bot, float top, float nearZ, float farZ)
	{
		this->camType = CAMERA_TYPE::ORTHOGRAPHIC;
		return new OrthographicCamera(
			position, lookAt,
			left, right,
			bot, top,
			nearZ, farZ);
	}
}
