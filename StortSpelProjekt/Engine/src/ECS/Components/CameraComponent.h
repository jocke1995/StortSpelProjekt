#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "Component.h"
#include "../../Renderer/PerspectiveCamera.h"
#include "../../Renderer/OrthographicCamera.h"
namespace component
{
	class CameraComponent : public Component
	{
	public:

		// Temp Constructor to handle input
		CameraComponent(Entity* parent, HINSTANCE hInstance, HWND hwnd, bool primary = false);
		
		// Default Settings
		CameraComponent(Entity* parent, CAMERA_TYPE camType, bool primary = false);

		// Perspective Constructor
		CameraComponent(Entity* parent,
			bool primary = false,
			XMVECTOR position = { 0.0f, 0.0f, 0.0f },
			XMVECTOR lookAt = { 1.0f, 1.0f, 1.0f },
			double fov = 45.0f,
			double aspectRatio = 16.0f / 9.0f,
			double zNear = 0.1f,
			double zFar = 1000.0f);

		// Orthographic Constructor
		CameraComponent(Entity* parent,
			bool primary = false,
			XMVECTOR position = { 0.0f, 0.0f, 0.0f },
			XMVECTOR lookAt = { 1.0f, 1.0f, 1.0f },
			float left = -40.0f,
			float right = 40.0f,
			float bot = -40.0f,
			float top = 40.0f,
			float nearZ = 0.01f,
			float farZ = 1000.0f);

		virtual ~CameraComponent();

		BaseCamera* GetCamera() const;
		bool IsPrimary() const;

		void Update(double dt);

	private:
		BaseCamera* camera = nullptr;
		CAMERA_TYPE camType = CAMERA_TYPE::UNDEFINED;
		bool primaryCamera = false;

		// Todo: add and calculate mesh to be able to draw frustrum in wireframe

		BaseCamera* CreatePerspective(
			XMVECTOR position = { 0.0f, 0.0f, 0.0f },
			XMVECTOR lookAt = { 1.0f, 1.0f, 1.0f },
			double fov = 45.0f,
			double aspectRatio = 16.0f / 9.0f,
			double zNear = 0.1f,
			double zFar = 1000.0f);

		BaseCamera* CreateOrthographic(
			XMVECTOR position = { 0.0f, 0.0f, 0.0f },
			XMVECTOR lookAt = { 1.0f, 1.0f, 1.0f },
			float left = -40.0f,
			float right = 40.0f,
			float bot = -40.0f,
			float top = 40.0f,
			float nearZ = 0.01f,
			float farZ = 1000.0f);
	};
}

#endif
