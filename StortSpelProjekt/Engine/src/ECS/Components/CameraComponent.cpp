#include "stdafx.h"
#include "CameraComponent.h"

// Renderer
#include "../../Renderer/PerspectiveCamera.h"
#include "../../Renderer/OrthographicCamera.h"

namespace component
{
	// Temp Constructor to handle input
	CameraComponent::CameraComponent(Entity* parent, HINSTANCE hInstance, HWND hwnd, bool primary)
		:Component(parent)
	{
		m_CamType = CAMERA_TYPE::PERSPECTIVE;
		m_PrimaryCamera = primary;
		m_pCamera = new PerspectiveCamera(hInstance, hwnd);
	}

	// Default Settings
	CameraComponent::CameraComponent(Entity* parent, CAMERA_TYPE camType, bool primary)
		:Component(parent)
	{
		m_PrimaryCamera = primary;

		switch (m_CamType)
		{
		case CAMERA_TYPE::PERSPECTIVE:
			m_pCamera = createPerspective();
			break;
		case CAMERA_TYPE::ORTHOGRAPHIC:
			m_pCamera = createOrthographic();
			break;
		}
	}

	// Perspective Constructor
	CameraComponent::CameraComponent(Entity* parent, bool primary, DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, double fov, double aspectRatio, double zNear, double zFar)
		: Component(parent)
	{
		m_PrimaryCamera = primary;
		m_pCamera = createPerspective(
			position, lookAt,
			fov, aspectRatio,
			zNear, zFar);
	}

	// Orthographic Constructor
	CameraComponent::CameraComponent(Entity* parent, bool primary, DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, float left, float right, float bot, float top, float nearZ, float farZ)
		: Component(parent)
	{
		m_PrimaryCamera = primary;
		m_pCamera = createOrthographic(
			position, lookAt,
			left, right, bot, top,
			nearZ, farZ);
	}

	CameraComponent::~CameraComponent()
	{
		delete m_pCamera;
	}

	BaseCamera* CameraComponent::GetCamera() const
	{
		return m_pCamera;
	}

	bool CameraComponent::IsPrimary() const
	{
		return m_PrimaryCamera;
	}

	void CameraComponent::Update(double dt)
	{
		m_pCamera->Update(dt);
	}

	BaseCamera* CameraComponent::createPerspective(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, double fov, double aspectRatio, double nearZ, double farZ)
	{
		m_CamType = CAMERA_TYPE::PERSPECTIVE;
		return new PerspectiveCamera(
			position, lookAt,
			fov, aspectRatio,
			nearZ, farZ);
	}

	BaseCamera* CameraComponent::createOrthographic(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, float left, float right, float bot, float top, float nearZ, float farZ)
	{
		m_CamType = CAMERA_TYPE::ORTHOGRAPHIC;
		return new OrthographicCamera(
			position, lookAt,
			left, right,
			bot, top,
			nearZ, farZ);
	}
}
