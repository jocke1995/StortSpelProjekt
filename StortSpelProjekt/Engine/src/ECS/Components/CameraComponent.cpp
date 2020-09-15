#include "stdafx.h"
#include "CameraComponent.h"
#include "..\Events\EventBus.h"

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

		m_Zoom = 12.0f;

		m_Height = 1.0f;

		m_CameraDistance = sqrt(m_Zoom * m_Zoom + (m_Zoom / 4) * (m_Zoom / 4));

		switch (m_CamType)
		{
		case CAMERA_TYPE::PERSPECTIVE:
			m_pCamera = createPerspective();
			break;
		case CAMERA_TYPE::ORTHOGRAPHIC:
			m_pCamera = createOrthographic();
			break;
		}

		// If player camera, subscribe to camera control events
		if (!std::strcmp(m_pParent->GetName().c_str(), "player"))
		{
			EventBus::GetInstance().Subscribe(this, &CameraComponent::toggleCameraLock);
			EventBus::GetInstance().Subscribe(this, &CameraComponent::zoom);
			EventBus::GetInstance().Subscribe(this, &CameraComponent::rotateCamera);
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
		(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? EventBus::GetInstance().Subscribe(this, &CameraComponent::moveCamera) : Log::Print("Unsubscribe\n")/* EventBus::GetInstance().Unsubscribe(this &CameraComponent::moveCamera)*/;
	}

	void CameraComponent::Update(double dt)
	{
		// Lock camera to player
		if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
		{
			Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
			float3 playerPosition = tc->GetPositionFloat3();

			DirectX::XMMATRIX rotMat = tc->GetRotMatrix();
			DirectX::XMFLOAT3 forward, right;
			DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
			DirectX::XMStoreFloat3(&right, rotMat.r[0]);

			float zoomBack = sqrt(m_CameraDistance * m_CameraDistance - ((m_Zoom / 4) * m_Height) * ((m_Zoom / 4) * m_Height));

			float3 cameraPosition = { playerPosition.x - (zoomBack * forward.x), playerPosition.y + (m_Zoom / 4) * m_Height, playerPosition.z - (zoomBack * forward.z) };

			m_pCamera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
			float directionX = playerPosition.x - cameraPosition.x;
			float directionY = playerPosition.y - cameraPosition.y;
			float directionZ = playerPosition.z - cameraPosition.z;
			m_pCamera->SetDirection(directionX, directionY, directionZ);
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

	void CameraComponent::toggleCameraLock(ModifierInput* evnt)
	{
		if (evnt->key == SCAN_CODES::LEFT_CTRL && evnt->pressed)
		{
			ToggleCameraLock();
		}
	}
	void CameraComponent::zoom(MouseScroll* evnt)
	{
		m_Zoom = std::max(m_Zoom - evnt->scroll, 6.0f);
		m_CameraDistance = sqrt(m_Zoom * m_Zoom + (m_Zoom / 4) * (m_Zoom / 4));
	}
	void CameraComponent::moveCamera(MovementInput* evnt)
	{

	}
	void CameraComponent::rotateCamera(MouseMovement* evnt)
	{
		// Mouse movement in x-direction
		int y = evnt->y;

		// Determine how much to rotate in radians
		float rotate = -(static_cast<float>(y) / 300.0) * 3.1415;

		m_Height = std::max(std::min(m_Height + rotate, 3.0f), -3.0f);
	}
}
