#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H
#include "EngineMath.h"
#include "Component.h"
#include "Core.h"
class BaseCamera;
class ModifierInput;
class MouseScroll;
class MovementInput;
class MouseMovement;

enum CAMERA_FLAGS
{
	// Set flag to make the camera position inherit the position of the corresponding m_pMesh
	USE_PLAYER_POSITION = BIT(1),
};

namespace component
{
	class InputComponent : public Component
	{
	public:
		// Default Settings
		InputComponent(Entity* parent, unsigned int camFlags = 0);

		virtual ~InputComponent();

		void Update(double dt);

	private:
		unsigned int m_CameraFlags = 0;
		float m_Zoom;
		float m_Pitch;
		float m_Yaw;
		float m_CameraDistance;

		void toggleCameraLock(ModifierInput* evnt);
		void zoom(MouseScroll* evnt);

		void move(MovementInput* evnt);
		void rotate(MouseMovement* evnt);
	};
}

#endif