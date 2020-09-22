#ifndef PLAYERINPUTCOMPONENT_H
#define PLAYERINPUTCOMPONENT_H
#include "EngineMath.h"
#include "..\ECS\Components\InputComponent.h"
#include "Core.h"
class BaseCamera;
class ModifierInput;
class MouseScroll;
class MovementInput;
class MouseMovement;
struct MouseClick;
class Transform;
class PerspectiveCamera;
class Collision;

enum CAMERA_FLAGS
{
	// Set flag to make the camera position inherit the position of the corresponding m_pMesh
	USE_PLAYER_POSITION = BIT(1),
};

namespace component
{
	class PlayerInputComponent : public InputComponent
	{
	public:
		// Default Settings
		PlayerInputComponent(Entity* parent, unsigned int camFlags = 0);

		virtual ~PlayerInputComponent();

		void Init();

		void Update(double dt);

	private:
		unsigned int m_CameraFlags = 0;
		float m_Zoom;
		float m_Pitch;
		float m_Yaw;
		float m_CameraDistance;
		PerspectiveCamera* m_pCamera;
		Transform* m_pTransform;

		void toggleCameraLock(ModifierInput* evnt);
		void zoom(MouseScroll* evnt);

		void move(MovementInput* evnt);
		void rotate(MouseMovement* evnt);

		void leftClick(MouseClick* evnt);

		void grunt(Collision* evnt);
	};
}

#endif