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

	m_Pitch = 0.15f;

	m_Yaw = 10.0f;

	m_JumpHeight = 5.0;
	m_JumpTime = 0.25;
	m_Gravity = (-2 * m_JumpHeight) / (m_JumpTime * m_JumpTime);
	m_MovementSpeed = 10.0f;

	m_pCamera = nullptr;
	m_pTransform = nullptr;
	m_pCC = nullptr;

	m_Dashing = false;
	m_DashReady = true;
	m_Attacking = false;

	m_Elevation = std::stof(Option::GetInstance().GetVariable("f_playerElevation"));

	specificUpdate = &PlayerInputComponent::updateDefault;
	specificUpdates.push_back(&PlayerInputComponent::updateDefault);

	m_UpdateShootId = -1;
	m_UpdateDashId = -1;

	m_AngleToTurnTo = 0;

	m_RotateX = 0.0f;
	m_RotateY = 0.0f;
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
		float3 cameraDir = m_pCamera->GetDirectionFloat3();
		float3 cameraPos = m_pCamera->GetPositionFloat3();

		cameraPos.y = min(max(cameraPos.y + m_RotateY * 100.0f * static_cast<float>(dt), -80.0f), 80.0f);
		m_pCamera->SetPosition(cameraPos.x, cameraPos.y, cameraPos.z);

		m_RotateX *= 100.0f * dt;

		cameraDir = {
			cameraDir.x * cos(m_RotateX) + cameraDir.z * sin(m_RotateX),
			playerPosition.y - cameraPos.y + (static_cast<float>(m_pParent->GetComponent<component::ModelComponent>()->GetModelDim().y) * m_pTransform->GetScale().y * 0.5f) + 1.0f,
			-cameraDir.x * sin(m_RotateX) + cameraDir.z * cos(m_RotateX)
		};

		m_pCamera->SetDirection(cameraDir.x, cameraDir.y, cameraDir.z);

		float3 forward = m_pCamera->GetDirectionFloat3();
		forward.normalize();
		forward *= 75.0f;
		float3 cameraPosition = playerPosition - forward;
		float height = (m_pParent->GetComponent<component::ModelComponent>()->GetModelDim().y * m_pTransform->GetScale().y * 0.5) + 1.0;
		cameraPosition.y += height;

		m_pCamera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
		float directionX = playerPosition.x - cameraPosition.x;
		float directionY = playerPosition.y - cameraPosition.y + height;
		float directionZ = playerPosition.z - cameraPosition.z;
		m_pCamera->SetDirection(directionX, directionY, directionZ);

		m_RotateX = m_RotateY = 0.0;
	}
	else
	{
		m_pCamera->SetDirection(cos(m_Yaw), m_Pitch * -2, sin(m_Yaw));
	}

	m_DashTimer += dt;
	m_DashReady = m_DashTimer > 1.5;
	
	/* Update model rotation */
	double3 rot = m_pCC->GetRotationEuler();
	int angle = EngineMath::convertToWholeDegrees(rot.y);
	int mod = EngineMath::convertToWholeDegrees(rot.x) / 180;

	angle = (mod * 360 + (mod * -2 + 1) * ((angle + 360) % 360) + mod * 180) % 360;

	int direction = static_cast<int>(((m_AngleToTurnTo - angle + 180) % 360 + 360) % 360 - 180 >= 0) * 2 - 1;

	if (angle != m_AngleToTurnTo && m_AngleToTurnTo != 360)
	{
		m_pCC->Rotate({ 0.0, 1.0, 0.0 }, direction * 5.0 * dt);
	}
	
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

void component::PlayerInputComponent::SetAngleToTurnTo(int angle)
{
	m_AngleToTurnTo = angle;
}

void component::PlayerInputComponent::SetAttacking()
{
	m_Attacking = true;
}

void component::PlayerInputComponent::Reset()
{
	EventBus::GetInstance().Unsubscribe(this, &PlayerInputComponent::alternativeInput);
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

void component::PlayerInputComponent::move(MovementInput* evnt)
{
	if (evnt->pressed)
	{
		m_Attacking = false;
	}

	double3 vel = m_pCC->GetLinearVelocity();

	// Check if the key has just been pressed or jsut been released and convert to a float. Multiply by two and subtract one to get 1 for true and -1 for false. If
	// the key has been pressed, the player should start moving in the direction specified by the key -- hence the value 1. If the key has been released, the player's
	// movement should be negated in the direction specified by the key -- hence the value -1
	double pressed = (static_cast<double>(evnt->pressed) * 2 - 1);

	// Find out which key has been pressed. Convert to float to get the value 1 if the key pressed should move the player in the positive
	// direction and the value -1 if the key pressed should move the player in the negative direction
	double3 moveCam =
	{
		(static_cast<double>(evnt->key == SCAN_CODES::D) - static_cast<double>(evnt->key == SCAN_CODES::A)) * pressed,
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
		/*float3 forward = m_pTransform->GetForwardFloat3();
		float3 right = m_pTransform->GetRightFloat3();*/
		float3 right = m_pCamera->GetRightVectorFloat3();
		float3 up = m_pCamera->GetUpVectorFloat3();
		float3 forward = right.cross(up);
		right.normalize();
		forward.normalize();

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

		if (std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON)
		{
			double angle = std::atan2(move.x, move.z);
			int angleDegrees = EngineMath::convertToWholeDegrees(angle);
			angleDegrees = (angleDegrees + 360) % 360;
			m_AngleToTurnTo = angleDegrees;
		}

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
	float rotateY = -(static_cast<float>(y) / 6.0 - static_cast<float>(y) / 3.0) * PI;
	float rotateX = -(static_cast<float>(x) / 800.0) * PI;

	m_Pitch = max(min(m_Pitch + rotateY, 3.0f), -3.0f);
	m_Yaw = m_Yaw + rotateX;

	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		rotateX = (static_cast<float>(x)) / 400.0 * PI;

		m_RotateX = rotateX;
		m_RotateY = rotateY;

		//Check if in air. If not, change movement direction to match up with camera direction
		if (m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75) != -1 && !m_Dashing)
		{
			// Get new direction
			float3 right = m_pCamera->GetRightVectorFloat3();
			float3 up = m_pCamera->GetUpVectorFloat3();
			float3 forward = right.cross(up);
			right.normalize();
			forward.normalize();

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

			if (std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON)
			{
				double angle;
				if (m_Attacking)
				{
					angle = std::atan2(forward.x, forward.z);
				}
				else
				{
					angle = std::atan2(move.x, move.z);
				}
				int angleDegrees = EngineMath::convertToWholeDegrees(angle);
				angleDegrees = (angleDegrees + 360) % 360;
				m_AngleToTurnTo = angleDegrees;
			}

			float3 playerDir = m_pTransform->GetForwardFloat3();
			float3 moveDir = { move.x, 0.0, move.z };
			moveDir.normalize();
			playerDir.normalize();

			float3 moveDif = moveDir - playerDir;

			bool movingForward = moveDif.length() <= 1.0;
			float speed = m_pTransform->GetVelocity() * (1 - 0.5 * static_cast<float>(!movingForward));

			// Update the player's velocity
			m_pCC->SetVelVector(move.x * speed, move.y, move.z * speed);
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

		double velY = vel.y;

		double3 move =
		{
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::A))),
			0.0,
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::S))),
		};

		float3 right = m_pCamera->GetRightVectorFloat3();
		float3 up = m_pCamera->GetUpVectorFloat3();
		float3 forward = right.cross(up);
		right.normalize();
		forward.normalize();

		move = {
			forward.x * move.z + right.x * move.x,
			vel.y,
			forward.z * move.z + right.z * move.x
		};

		vel = {
			move.x * m_pTransform->GetVelocity(),
			velY,
			move.z * m_pTransform->GetVelocity()
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
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::A))),
			0.0,
			(static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<double>(Input::GetInstance().GetKeyState(SCAN_CODES::S))),
		};

		float3 right = m_pCamera->GetRightVectorFloat3();
		float3 up = m_pCamera->GetUpVectorFloat3();
		float3 forward = right.cross(up);
		right.normalize();
		forward.normalize();

		move = {
			forward.x * move.z + right.x * move.x,
			0.0,
			forward.z * move.z + right.z * move.x
		};
		move.normalize();

		double3 vel = {
			move.x * m_pTransform->GetVelocity(),
			0.0,
			move.z * m_pTransform->GetVelocity()
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
