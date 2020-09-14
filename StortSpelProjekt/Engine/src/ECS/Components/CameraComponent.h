#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "EngineMath.h"
#include "Component.h"
#include "Core.h"
class BaseCamera;
class ModifierInput;
class MouseScroll;

enum CAMERA_FLAGS
{
	// Set flag to make the camera position inherit the position of the corresponding m_pMesh
	USE_PLAYER_POSITION = BIT(1),
};

namespace component
{
	class CameraComponent : public Component
	{
	public:
		// Temp Constructor to handle input
		CameraComponent(Entity* parent, HINSTANCE hInstance, HWND hwnd, bool primary = false);
		
		// Default Settings
		CameraComponent(Entity* parent, CAMERA_TYPE camType, bool primary = false, unsigned int camFlags = 0);

		// Perspective Constructor
		CameraComponent(Entity* parent,
			bool primary = false,
			DirectX::XMVECTOR position = { 0.0, 4.0, -10.0 },
			DirectX::XMVECTOR direction = { 0.0f, -2.0f, 10.0f },
			double fov = 45.0f,
			double aspectRatio = 16.0f / 9.0f,
			double zNear = 0.1f,
			double zFar = 1000.0f);

		// Orthographic Constructor
		CameraComponent(Entity* parent,
			bool primary = false,
			DirectX::XMVECTOR position = { 0.0, 4.0, -10.0 },
			DirectX::XMVECTOR direction = { 0.0f, -2.0f, 10.0f },
			float left = -40.0f,
			float right = 40.0f,
			float bot = -40.0f,
			float top = 40.0f,
			float nearZ = 0.01f,
			float farZ = 1000.0f);

		virtual ~CameraComponent();

		BaseCamera* GetCamera() const;
		bool IsPrimary() const;

		/// <summary>
		/// Toggles whether the camera locks to the movement of the player
		/// </summary>

		void Update(double dt);
		void ToggleCameraLock();

	private:
		BaseCamera* m_pCamera = nullptr;
		CAMERA_TYPE m_CamType = CAMERA_TYPE::UNDEFINED;
		bool m_PrimaryCamera = false;
		unsigned int m_CameraFlags = 0;
		float m_Zoom;

		// Todo: add and calculate m_pMesh to be able to draw frustrum in wireframe

		BaseCamera* createPerspective(
			DirectX::XMVECTOR position = { 0.0, 4.0, -10.0 },
			DirectX::XMVECTOR direction = { 0.0f, -2.0f, 10.0f },
			double fov = 45.0f,
			double aspectRatio = 16.0f / 9.0f,
			double zNear = 0.1f,
			double zFar = 1000.0f);

		BaseCamera* createOrthographic(
			DirectX::XMVECTOR position = { 0.0, 4.0, -10.0 },
			DirectX::XMVECTOR direction = { 0.0f, -2.0f, 10.0f },
			float left = -40.0f,
			float right = 40.0f,
			float bot = -40.0f,
			float top = 40.0f,
			float nearZ = 0.01f,
			float farZ = 1000.0f);

		void toggleCameraLock(ModifierInput* evnt);
		void zoom(MouseScroll* evnt);
	};
}

#endif
