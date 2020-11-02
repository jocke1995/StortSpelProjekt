#include "PlayerInputComponent.h"
#include "Components/MeleeComponent.h"
#include "../Events/EventBus.h"
#include "../ECS/Entity.h"
#include "../Renderer/Camera/PerspectiveCamera.h"
#include "../Renderer/Transform.h"
#include "../ECS/Components/Collision/CollisionComponent.h"
#include "Physics/Physics.h"
#include "../Misc/Option.h"


component::PlayerInputComponent::PlayerInputComponent(Entity* parent, unsigned int camFlags)
	:InputComponent(parent)
{
	m_DashTimer = 0;
	m_CameraFlags = camFlags;

	m_Zoom = 16.0f;

	m_Pitch = 0.15f;

	m_Yaw = 10.0f;

	m_JumpHeight = 5.0;
	m_JumpTime = 0.25;
	m_Gravity = (-2 * m_JumpHeight) / (m_JumpTime * m_JumpTime);

	m_CameraDistance = sqrt(m_Zoom * 4 * m_Zoom * 4 + m_Zoom * m_Zoom);

	m_pCamera = nullptr;
	m_pTransform = nullptr;
	m_pCC = nullptr;

	m_Dashing = false;
	m_DashReady = true;

	m_Elevation = std::stof(Option::GetInstance().GetVariable("f_playerElevation"));

	specificUpdate = &PlayerInputComponent::updateDefault;
	specificUpdates.push_back(&PlayerInputComponent::updateDefault);

	m_UpdateShootId = -1;
	m_UpdateDashId = -1;
}

component::PlayerInputComponent::~PlayerInputComponent()
{
}

void component::PlayerInputComponent::Init()
{
	
}

void component::PlayerInputComponent::OnInitScene()
{
	m_pCamera = static_cast<PerspectiveCamera*>(m_pParent->GetComponent<component::CameraComponent>()->GetCamera());
	m_pTransform = static_cast<Transform*>(m_pParent->GetComponent<component::TransformComponent>()->GetTransform());

	m_pTransform->SetVelocity(m_MovementSpeed);

	m_pCC = m_pParent->GetComponent<component::CollisionComponent>();

	// TODO: Unsubrscibe somewhere
	if (m_pCC && m_pCamera && m_pTransform)
	{
		EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::alternativeInput);
		EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::zoom);
		EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::rotate);
		EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::move);
		if (m_pParent->GetComponent<component::MeleeComponent>() != nullptr)
		{
			EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::mouseClick);
		}
	}

	if (!m_pCC)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "PlayerInputComponent needs a collision component!\n");
	}

	if (!m_pCamera)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "PlayerInputComponent needs a Camera component!\n");
	}

	if (!m_pTransform)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "PlayerInputComponent needs a Transform component!\n");
	}
}

void component::PlayerInputComponent::OnUnInitScene()
{
}

void component::PlayerInputComponent::RenderUpdate(double dt)
{
	// Lock camera to player
	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		float3 playerPosition = m_pTransform->GetRenderPositionFloat3();
		float3 forward = m_pTransform->GetForwardFloat3();

		float zoomBack = sqrt(m_CameraDistance * m_CameraDistance - (m_Zoom * m_Pitch) * (m_Zoom * m_Pitch));

		float3 cameraPosition = { playerPosition.x - (zoomBack * forward.x), playerPosition.y + m_Zoom * m_Pitch, playerPosition.z - (zoomBack * forward.z) };

		m_pCamera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
		float directionX = playerPosition.x - cameraPosition.x;
		float directionY = playerPosition.y + (m_pParent->GetComponent<component::ModelComponent>()->GetModelDim().y * m_pTransform->GetScale().y * 0.5) + 1.0 - cameraPosition.y;
		float directionZ = playerPosition.z - cameraPosition.z;
		m_pCamera->SetDirection(directionX, directionY, directionZ);
	}
	else
	{
		m_pCamera->SetDirection(cos(m_Yaw), m_Pitch * -2, sin(m_Yaw));
	}

	m_DashTimer += dt;
	m_DashReady = m_DashTimer > 1.5;
	for (unsigned int i = 0; i < specificUpdates.size(); ++i)
	{
		specificUpdate = specificUpdates.at(i);
		(this->*specificUpdate)(dt);
	}
}

void component::PlayerInputComponent::SetJumpHeight(double height)
{
	m_JumpHeight = height;
	m_Gravity = (-2 * m_JumpHeight) / (m_JumpTime * m_JumpTime);
}

void component::PlayerInputComponent::SetJumpTime(double time)
{
	m_JumpTime = time;
	m_Gravity = (-2 * m_JumpHeight) / (m_JumpTime * m_JumpTime);
}

void component::PlayerInputComponent::SetMovementSpeed(float speed)
{
	m_MovementSpeed = speed;
}

void component::PlayerInputComponent::Reset()
{
	EventBus::GetInstance().Unsubscribe(this, &PlayerInputComponent::alternativeInput);
	EventBus::GetInstance().Unsubscribe(this, &PlayerInputComponent::zoom);
	EventBus::GetInstance().Unsubscribe(this, &PlayerInputComponent::rotate);
	EventBus::GetInstance().Unsubscribe(this, &PlayerInputComponent::move);
	if (m_pParent->GetComponent<component::MeleeComponent>() != nullptr)
	{
		EventBus::GetInstance().Unsubscribe(this, &PlayerInputComponent::mouseClick);
	}
}

void component::PlayerInputComponent::alternativeInput(ModifierInput* evnt)
{
	if (evnt->key == SCAN_CODES::LEFT_CTRL && evnt->pressed)
	{
		// XOR operator. If m_CameraFlags contains the flag, remove it. Otherwise add it to m_CameraFlags.
		m_CameraFlags ^= CAMERA_FLAGS::USE_PLAYER_POSITION;
		m_pCamera->SetMovement(0.0f, 0.0f, 0.0f);
		m_pTransform->SetMovement(0.0f, 0.0f, 0.0f);
		m_Yaw = sqrt(2);
		m_Pitch = 0.0f;
	}
	// If shift is pressed and held, increase velocity (Number not set in stone)
	else if (evnt->key == SCAN_CODES::LEFT_SHIFT && evnt->pressed)
	{
		m_pTransform->SetVelocity(SPRINT_MOD * m_MovementSpeed);
		// Check if the player is in the air. If not, allow sprint
		if (m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75) != -1)
		{
			// Get the current linear velocity of the player
			double3 vel = m_pCC->GetLinearVelocity();
			vel *= SPRINT_MOD;
			m_pCC->SetVelVector(vel.x, vel.y, vel.z);
		}
	}
	// If shift is released, decrease velocity to "normal" values
	else if (evnt->key == SCAN_CODES::LEFT_SHIFT && !evnt->pressed)
	{
		m_pTransform->SetVelocity(m_MovementSpeed);
		// Check if the player is in the air. If not, allow sprint
		if (m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75) != -1)
		{
			// Get the current linear velocity of the player
			double3 vel = m_pCC->GetLinearVelocity();
			vel /= SPRINT_MOD;
			m_pCC->SetVelVector(vel.x, vel.y, vel.z);
		}
	}
}

void component::PlayerInputComponent::zoom(MouseScroll* evnt)
{
	m_Zoom = max(m_Zoom - static_cast<float>(evnt->scroll) / 4, 1.5f);
	m_CameraDistance = sqrt(m_Zoom * 4 * m_Zoom * 4 + m_Zoom * m_Zoom);
}

void component::PlayerInputComponent::move(MovementInput* evnt)
{
	double3 vel = m_pCC->GetLinearVelocity();

	// Check if the key has just been pressed or jsut been released and convert to a float. Multiply by two and subtract one to get 1 for true and -1 for false. If
	// the key has been pressed, the player should start moving in the direction specified by the key -- hence the value 1. If the key has been released, the player's
	// movement should be negated in the direction specified by the key -- hence the value -1
	double pressed = (static_cast<double>(evnt->pressed) * 2 - 1);

	// Find out which key has been pressed. Convert to float to get the value 1 if the key pressed should move the player in the positive
	// direction and the value -1 if the key pressed should move the player in the negative direction
	double3 moveCam =
	{
		(static_cast<double>(evnt->key == SCAN_CODES::A) - static_cast<double>(evnt->key == SCAN_CODES::D)) * pressed,
		(static_cast<double>(evnt->key == SCAN_CODES::Q) - static_cast<double>(evnt->key == SCAN_CODES::E)) * pressed,
		(static_cast<double>(evnt->key == SCAN_CODES::W) - static_cast<double>(evnt->key == SCAN_CODES::S)) * pressed,
	};

	// Check if the player is in the air. If not, allow movement
	double dist = m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75);
	if (dist != -1 && !m_Dashing)
	{
		double moveRight = (static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::A)));
		double moveForward = (static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::S)));
		bool dash = (evnt->key == SCAN_CODES::E || evnt->key == SCAN_CODES::Q) && evnt->pressed;

		double jump = static_cast<double>(evnt->key == SCAN_CODES::SPACE) * static_cast<double>(evnt->pressed);

		// Get the forward and right vectors to determine in which direction to move
		float3 forward = m_pTransform->GetForwardFloat3();
		float3 right = m_pTransform->GetRightFloat3();

		double3 move = {
			forward.x * moveForward + right.x * moveRight,
			0.0,
			forward.z * moveForward + right.z * moveRight
		};

 		move.normalize();
		// Get the current linear velocity of the player
		vel =
		{
			move.x * m_pTransform->GetVelocity(),
			//Constant value to compensate for sprint velocity
			jump * ((2*m_JumpHeight) / (m_JumpTime)),
			move.z * m_pTransform->GetVelocity()
		};

		bool wasDashing = m_Dashing;
		m_Dashing = m_DashReady && dash && static_cast<double>(evnt->key != SCAN_CODES::SPACE);
		if (m_Dashing)
		{
			m_DashTimer = 0;
			if (vel == double3({ 0.0, 0.0, 0.0 }))
			{
				forward.normalize();
				vel = { forward.x * m_MovementSpeed * DASH_MOD, forward.y, forward.z * m_MovementSpeed * DASH_MOD };
			}
			else
			{
				vel.normalize();
				vel *= m_MovementSpeed * DASH_MOD;
			}
			m_UpdateDashId = specificUpdates.size();
			specificUpdates.push_back(&PlayerInputComponent::updateDash);
		}
		else
		{
			m_Dashing = wasDashing;
		}

	}
	else if (evnt->key == SCAN_CODES::SPACE && !evnt->pressed)
	{
 		specificUpdates.at(0) = &PlayerInputComponent::updateJump;
		m_pCC->SetGravity(m_Gravity);
	}

	moveCam *= m_pTransform->GetVelocity() / 5.0;
	// If the camera uses the players position, update the player's velocity. Otherwise update the camera's movement.
	(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? m_pCC->SetVelVector(vel.x, vel.y, vel.z) : m_pCamera->UpdateMovement(moveCam.x, moveCam.y, moveCam.z);
}

void component::PlayerInputComponent::rotate(MouseMovement* evnt)
{
	// Rotate camera
	// Mouse movement
	int x = evnt->x, y = evnt->y;

	// Determine how much to rotate in radians
	double rotateY = (static_cast<double>(y) / 600.0) * PI;
	double rotateX = -(static_cast<double>(x) / 800.0) * PI;

	m_Pitch = max(min(m_Pitch + rotateY, 3.0f), -3.0f);
	m_Yaw = m_Yaw + rotateX;

	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		// Rotate transform
		// Determine how much to rotate in radians
		rotateX = (static_cast<double>(x)) / 400.0 * PI;

		// Get forawrd vector to determine current rotation angle
		float3 forward = m_pTransform->GetForwardFloat3();
		float angle = std::atan2(forward.x, forward.z);

		// Set the new rotation
		m_pCC->Rotate({ 0.0, 1.0, 0.0 }, rotateX);
		m_pCC->SetAngularVelocity(0.0, 0.0, 0.0);

		//Check if in air. If not, change movement direction to match up with camera direction
		if (m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75) != -1 && !m_Dashing)
		{
			// Get new direction
			forward = m_pTransform->GetForwardFloat3();
			float3 right = m_pTransform->GetRightFloat3();

			// Determine if player is currently moving, if yes, update movement direction
			double isMovingZ = static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::S));
			double isMovingX = static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::A));

			// Get the current linear velocity of the player
			double3 vel = m_pCC->GetLinearVelocity();
			double3 move = {
				forward.x * isMovingZ + right.x * isMovingX,
				0.0,
				forward.z * isMovingZ + right.z * isMovingX
			};
			move.normalize();
			move.y = vel.y;

			// Update the player's velocity
			m_pCC->SetVelVector(move.x * m_pTransform->GetVelocity(), move.y, move.z * m_pTransform->GetVelocity());
		}
	}
}

void component::PlayerInputComponent::mouseClick(MouseClick* evnt)
{
	switch (evnt->button) {
	case MOUSE_BUTTON::LEFT_DOWN:
		m_pParent->GetComponent<component::MeleeComponent>()->Attack();
		break;
	case MOUSE_BUTTON::RIGHT_DOWN:
		m_pParent->GetComponent<component::RangeComponent>()->Attack();
		m_UpdateShootId = specificUpdates.size();
		specificUpdates.push_back(&PlayerInputComponent::updateShoot);
		break;
	case MOUSE_BUTTON::RIGHT_UP:
		if (m_UpdateShootId != -1)
		{
			specificUpdates.erase(specificUpdates.begin() + m_UpdateShootId);
		}
		break;
	}
}

void component::PlayerInputComponent::updateDefault(double dt)
{
	double distanceToBottom = m_pCC->GetDistanceToBottom() + m_Elevation;
	double distanceToGround = m_pCC->CastRay({ 0.0, -1.0, 0.0 }, distanceToBottom);
	if (distanceToGround != -1)
	{
		double3 pos = m_pCC->GetPosition();
		pos.y = pos.y - distanceToGround + distanceToBottom - m_Elevation * 0.5;
		m_pCC->SetPosition(pos.x,pos.y,pos.z);
		m_pCC->SetGravity(0);
	}
	else
	{
		m_pCC->SetGravity(m_Gravity);
	}
}

void component::PlayerInputComponent::updateDash(double dt)
{
	if (m_DashTimer > 0.3 && m_Dashing)
	{
		double3 vel = m_pCC->GetLinearVelocity();

		double3 move =
		{
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::A))) * m_pTransform->GetVelocity(),
			vel.y,
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::S))) * m_pTransform->GetVelocity(),
		};

		float3 forward = m_pTransform->GetForwardFloat3();
		float3 right = m_pTransform->GetRightFloat3();

		vel = {
			forward.x * move.z + right.x * move.x,
			vel.y,
			forward.z * move.z + right.z * move.x
		};
		m_pCC->SetVelVector(vel.x, vel.y, vel.z);
		m_Dashing = false;

		if (m_UpdateDashId != -1)
		{
			specificUpdates.erase(specificUpdates.begin() + m_UpdateDashId);
		}
	}
}

void component::PlayerInputComponent::updateJump(double dt)
{
	if (m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75) != -1)
	{
		double3 move =
		{
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::A))) * m_pTransform->GetVelocity(),
			0.0,
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::S))) * m_pTransform->GetVelocity(),
		};

		float3 forward = m_pTransform->GetForwardFloat3();
		float3 right = m_pTransform->GetRightFloat3();

		double3 vel = {
			forward.x * move.z + right.x * move.x,
			move.y,
			forward.z * move.z + right.z * move.x
		};
		m_pCC->SetVelVector(vel.x, vel.y, vel.z);

		specificUpdates.at(0) = &PlayerInputComponent::updateDefault;
	}
}

void component::PlayerInputComponent::updateShoot(double dt)
{
	if (Input::GetInstance().GetMouseButtonState(MOUSE_BUTTON::RIGHT_DOWN))
	{
		m_pParent->GetComponent<component::RangeComponent>()->Attack();
	}
}
