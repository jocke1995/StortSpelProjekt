#include "stdafx.h"
#include "CameraComponent.h"

// Renderer
#include "../../Renderer/PerspectiveCamera.h"
#include "../../Renderer/OrthographicCamera.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"

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
	CameraComponent::CameraComponent(Entity* parent, CAMERA_TYPE camType, bool primary, unsigned int camFlags)
		:Component(parent)
	{
		m_PrimaryCamera = primary;

		m_CameraFlags = camFlags;

		m_CamType = camType;

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
	CameraComponent::CameraComponent(Entity* parent, bool primary, DirectX::XMVECTOR position, DirectX::XMVECTOR direction, double fov, double aspectRatio, double zNear, double zFar)
		: Component(parent)
	{
		m_PrimaryCamera = primary;
		m_pCamera = createPerspective(
			position, direction,
			fov, aspectRatio,
			zNear, zFar);
	}

	// Orthographic Constructor
	CameraComponent::CameraComponent(Entity* parent, bool primary, DirectX::XMVECTOR position, DirectX::XMVECTOR direction, float left, float right, float bot, float top, float nearZ, float farZ)
		: Component(parent)
	{
		m_PrimaryCamera = primary;
		m_pCamera = createOrthographic(
			position, direction,
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

	void CameraComponent::ToggleCameraLock()
	{
		m_CameraFlags ^= CAMERA_FLAGS::USE_PLAYER_POSITION;
	}

	void CameraComponent::Update(double dt)
	{
		if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
		{
			Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
			float3 position = tc->GetPositionFloat3();
			m_pCamera->SetPosition(position.x, position.y + 2, position.z - 10);
			m_pCamera->SetDirection(0.0, -2.0, 10.0);
		}

		m_pCamera->Update(dt);
	}

	BaseCamera* CameraComponent::createPerspective(DirectX::XMVECTOR position, DirectX::XMVECTOR direction, double fov, double aspectRatio, double nearZ, double farZ)
	{
		m_CamType = CAMERA_TYPE::PERSPECTIVE;
		return new PerspectiveCamera(
			position, direction,
			fov, aspectRatio,
			nearZ, farZ);
	}

	BaseCamera* CameraComponent::createOrthographic(DirectX::XMVECTOR position, DirectX::XMVECTOR direction, float left, float right, float bot, float top, float nearZ, float farZ)
	{
		m_CamType = CAMERA_TYPE::ORTHOGRAPHIC;
		return new OrthographicCamera(
			position, direction,
			left, right,
			bot, top,
			nearZ, farZ);
	}
}
