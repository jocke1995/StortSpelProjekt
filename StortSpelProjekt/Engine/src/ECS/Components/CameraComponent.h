#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "EngineMath.h"
#include "Component.h"
#include "Core.h"
class BaseCamera;

namespace component
{
	class CameraComponent : public Component
	{
	public:
		// Default Settings
		CameraComponent(Entity* parent, CAMERA_TYPE camType, bool primary = false);

		virtual ~CameraComponent();

		BaseCamera* GetCamera() const;
		bool IsPrimary() const;

		void RenderUpdate(double dt);
		void Init(Renderer* renderer);

	private:
		BaseCamera* m_pCamera = nullptr;
		CAMERA_TYPE m_CamType = CAMERA_TYPE::UNDEFINED;
		bool m_PrimaryCamera = false;

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
	};
}

#endif
