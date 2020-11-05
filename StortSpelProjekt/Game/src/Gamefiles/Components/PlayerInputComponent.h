#ifndef PLAYERINPUTCOMPONENT_H
#define PLAYERINPUTCOMPONENT_H
#include "EngineMath.h"
#include "..\ECS\Components\InputComponent.h"
#include "Core.h"
#include <functional>

#define DASH_MOD 3.0
#define SPRINT_MOD 1.5
#define TURN_RATE 20.0

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

		void OnInitScene();
		void OnUnInitScene();

		void RenderUpdate(double dt);

		//Sets the maximum height of a jump
		void SetJumpHeight(double height);
		//Sets the time until maximum height. Total time is twice
		void SetJumpTime(double time);
		//Sets the movement speed
		void SetMovementSpeed(float speed);

		void SetAngleToTurnTo(int angle);

		void SetAttacking();

		void Reset();

	private:
		unsigned int m_CameraFlags = 0;
		float m_Pitch;
		float m_Yaw;
		float m_Elevation;
		float m_MovementSpeed;
		float m_RotateX;
		float m_RotateY;

		PerspectiveCamera* m_pCamera;
		Transform* m_pTransform;

		CollisionComponent* m_pCC;

		double m_DashTimer;
		bool m_DashReady;
		bool m_Dashing;

		// Is used to determine if the player is attacking, and should be turned in the camera direction, or if she should turn in the direction she is moving
		bool m_Attacking;
		bool m_TurnToCamera;
		bool m_CameraRotating;

		double m_JumpHeight;
		double m_JumpTime;
		double m_Gravity;

		int m_UpdateShootId;
		int m_UpdateDashId;
		int m_DegreesToTurnTo;
		float m_RadiansToTurn;

		std::vector<void(PlayerInputComponent::*)(double dt)> specificUpdates;
		void(PlayerInputComponent::*specificUpdate)(double dt);


		void alternativeInput(ModifierInput* evnt);

		void move(MovementInput* evnt);
		void rotate(MouseMovement* evnt);

		void mouseClick(MouseClick* evnt);

		void updateDefault(double dt);
		void updateDash(double dt);
		void updateJump(double dt);
		void updateShoot(double dt);
	};
}

#endif