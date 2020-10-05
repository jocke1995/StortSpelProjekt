#ifndef PLAYERINPUTCOMPONENT_H
#define PLAYERINPUTCOMPONENT_H
#include "EngineMath.h"
#include "..\ECS\Components\InputComponent.h"
#include "Core.h"
#include <chrono>

//Camera
class BaseCamera;
class Transform;
class PerspectiveCamera;
//Collision
class Collision;
//Events
struct MouseScroll;
struct MovementInput;
struct MouseMovement;
struct MouseClick;
struct ModifierInput;

enum CAMERA_FLAGS
{
	// Set flag to make the camera position inherit the position of the corresponding m_pMesh
	USE_PLAYER_POSITION = BIT(1),
};

namespace component
{
	class CollisionComponent;

	class PlayerInputComponent : public InputComponent
	{
	public:
		// Default Settings
		PlayerInputComponent(Entity* parent, unsigned int camFlags = 0);

		virtual ~PlayerInputComponent();

		void Init();

		void RenderUpdate(double dt);

	private:
		unsigned int m_CameraFlags = 0;
		float m_Zoom;
		float m_Pitch;
		float m_Yaw;
		float m_CameraDistance;
		PerspectiveCamera* m_pCamera;
		Transform* m_pTransform;

		CollisionComponent* m_pCC;

		std::chrono::system_clock::time_point m_DashTimer;
		bool m_DashReady;
		bool m_Dashing;

		void toggleCameraLock(ModifierInput* evnt);
		void zoom(MouseScroll* evnt);

		void move(MovementInput* evnt);
		void rotate(MouseMovement* evnt);

		void mouseClick(MouseClick* evnt);
	};
}

#endif