#include "PlayerInputComponent.h"
#include "Components/MeleeComponent.h"
#include "../Events/EventBus.h"
#include "../ECS/Entity.h"
#include "../Renderer/Camera/PerspectiveCamera.h"
#include "../Renderer/Transform.h"
#include "../ECS/Components/Collision/CollisionComponent.h"
#include "Physics/Physics.h"
#include "../Misc/Option.h"
#include "Shop.h"
#include "stdafx.h"

#include "Player.h"


component::PlayerInputComponent::PlayerInputComponent(Entity* parent, unsigned int camFlags)
	:InputComponent(parent)
{
	m_DashTimer = 0;
	m_CameraFlags = camFlags;

	m_Pitch = 0.0f;
	m_Yaw = 0.0f;

	m_CameraDistance = ORIGINAL_CAMERA_DISTANCE;
	m_Height = -1;
	
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
	m_TurnToCamera = false;
	m_CameraRotating = false;

	m_Elevation = std::stof(Option::GetInstance().GetVariable("f_playerElevation"));

	specificUpdate = &PlayerInputComponent::updateDefault;
	specificUpdates.push_back(&PlayerInputComponent::updateDefault);

	m_UpdateShootId = -1;
	m_UpdateDashId = -1;

	m_TurningTimer = 0.0f;
	m_TurningInterval = 0.0f;
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

	component::RangeComponent* rc = m_pParent->GetComponent<component::RangeComponent>();

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
		if (rc)
		{
			m_TurningInterval = rc->GetAttackInterval() * 1.5;
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

	if (!rc)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "PlayerInputComponent needs a Range component for proper functionality!\n");
	}
}

void component::PlayerInputComponent::OnUnInitScene()
{
}

void component::PlayerInputComponent::RenderUpdate(double dt)
{
	// TODO: since it is constant, only calculate this once.
	// As of writing this, crashes if run on OnInit()
	m_Height = (m_pParent->GetComponent<component::ModelComponent>()->GetModelDim().y * m_pTransform->GetScale().y * 0.5) + 1.0;

	// Lock camera to player
	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		updateCameraDirection();

		setCameraToPlayerPosition();

		limitCameraDistance();

		double3 vel = m_pCC->GetLinearVelocity();
		float3 camDir = m_pCamera->GetDirectionFloat3();

		// USed to rotate player when rotating camera while moving
		if (m_Attacking || m_TurnToCamera)
		{
			float angle = std::atan2(m_pTransform->GetInvDir() * camDir.x, m_pTransform->GetInvDir() * camDir.z);
			m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
		}
		else if ((std::abs(vel.x) > EPSILON || std::abs(vel.y) > EPSILON) && m_CameraRotating)
		{
			float angle = std::atan2(m_pTransform->GetInvDir() * vel.x, m_pTransform->GetInvDir() * vel.z);
			m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
		}
	}
	else
	{
		updateCameraDirection();
	}

	/* ------------------ Increment timers -------------------- */

	m_TurningTimer += dt;
	m_DashTimer += dt;
	m_DashReady = m_DashTimer > 1.5;
	
	// Call the specific update functions
	for (unsigned int i = 0; i < specificUpdates.size(); ++i)
	{
		specificUpdate = specificUpdates.at(i);
		(this->*specificUpdate)(dt);
	}

	if (m_TurningTimer >= m_TurningInterval && m_Attacking)
	{
		m_Attacking = false;
		double3 vel = m_pCC->GetLinearVelocity();
		float speed = m_pTransform->GetVelocity();
		double3 move = {
			vel.x,
			0.0,
			vel.z
		};
		move.normalize();
		if (!m_Dashing)
		{
			m_pCC->SetVelVector(move.x * speed, vel.y, move.z * speed);
		}
		float3 right = m_pCamera->GetRightVectorFloat3();
		float3 up = m_pCamera->GetUpVectorFloat3();
		float3 forward = right.cross(up);
		forward.normalize();

		if (std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON)
		{
			double angle = std::atan2(m_pTransform->GetInvDir() * move.x, m_pTransform->GetInvDir() * move.z);
			m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
		}
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
	m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
}

void component::PlayerInputComponent::SetAttacking()
{
	m_Attacking = true;
	m_TurningTimer = 0.0f;
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

void component::PlayerInputComponent::updateCameraDirection()
{
	// yaw/pitch/roll conversion to quaternion
	// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	// Abbreviations for the various angular functions
	double cy = cos(0 * 0.5);
	double sy = sin(0 * 0.5);
	double cp = cos(-m_Yaw * 0.5);
	double sp = sin(-m_Yaw * 0.5);
	double cr = cos(m_Pitch * 0.5);
	double sr = sin(m_Pitch * 0.5);

	DirectX::XMVECTOR rotQuaternion = {
		sr * cp * cy - cr * sp * sy,
		cr * sp * cy + sr * cp * sy,
		cr * cp * sy - sr * sp * cy,
		cr * cp * cy + sr * sp * sy
	};

	DirectX::XMVECTOR rotatedDir;
	// Rotate around quaternion axis
	rotatedDir = DirectX::XMVector3Rotate({ 0, 0, 1 }, rotQuaternion);

	DirectX::XMFLOAT3 newDir;
	DirectX::XMStoreFloat3(&newDir, rotatedDir);

	m_pCamera->SetDirection(newDir.x, newDir.y, newDir.z);
}

void component::PlayerInputComponent::setCameraToPlayerPosition()
{
	// Move camera to player with a position offset (determined by forward and height)
	float3 playerPosition = m_pTransform->GetPositionFloat3();

	float3 forward = m_pCamera->GetDirectionFloat3();
	forward.normalize();
	forward *= m_CameraDistance;
	float3 cameraPosition = playerPosition - forward;
	cameraPosition.y += m_Height;

	m_pCamera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
}

void component::PlayerInputComponent::limitCameraDistance()
{
	float3 camDir = m_pCamera->GetDirectionFloat3();

	// Move camera closer if there is a wall closer
	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		double3 negCameraDir = {
			-camDir.x,
			-camDir.y,
			-camDir.z };
		double dist = m_pCC->CastRay(1, negCameraDir, ORIGINAL_CAMERA_DISTANCE, { 0 , m_Height , 0});
		if (dist != -1)
		{
			m_CameraDistance = abs(dist - 3);
		}
		else
		{
			m_CameraDistance = ORIGINAL_CAMERA_DISTANCE;
		}
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
	// If press tab, toggle between free direction or direction locked to camera
	else if (evnt->key == SCAN_CODES::TAB && evnt->pressed)
	{
		m_TurnToCamera = !m_TurnToCamera;
	}
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
		bool dash = (evnt->key == SCAN_CODES::LEFT_SHIFT || evnt->key == SCAN_CODES::RIGHT_SHIFT) && evnt->pressed;

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

		// If player is moving, turn in the direction of movement
		if (std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON)
		{
			m_pParent->GetComponent<component::AnimationComponent>()->PlayAnimation("Run", true);

			double angle = std::atan2(m_pTransform->GetInvDir() * move.x, m_pTransform->GetInvDir() * move.z);
			double forwardAngle = std::atan2(m_pTransform->GetInvDir() * forward.x, m_pTransform->GetInvDir() * forward.z);
			if (m_Attacking || m_TurnToCamera)
			{
				// If the player is in attacking position, turn in the camera direction
				angle = forwardAngle;
			}
			m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
		}
		else
		{
			m_pParent->GetComponent<component::AnimationComponent>()->PlayAnimation("Idle", true);

			double angle = std::atan2(m_pTransform->GetInvDir() * vel.x, m_pTransform->GetInvDir() * vel.z);
			m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
		}

		// Check if the player is moving in the direction she is turned. If not, lower the movement speed
		float3 playerDir = m_pTransform->GetForwardFloat3();
		float3 moveDir = { move.x, 0.0, move.z };
		moveDir.normalize();
		playerDir.normalize();

		float speed = m_pTransform->GetVelocity();
		if ((std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON) && (m_Attacking || m_TurnToCamera))
		{
			// Check if the player is moving in the direction she is turned. If not, lower the movement speed
			float3 playerDir = m_pTransform->GetForwardFloat3();
			float3 moveDir = { move.x, 0.0, move.z };
			moveDir.normalize();
			playerDir.normalize();

			float moveDif = EngineMath::convertToDegrees(moveDir.angle(playerDir));

			speed *= (1.0f - (moveDif / (180.0f / SLOWDOWN_FACTOR)));
		}

		// Get the current linear velocity of the player
		vel =
		{
			move.x * speed,
			//Constant value to compensate for sprint velocity
			jump * ((2*m_JumpHeight) / (m_JumpTime)),
			move.z * speed
		};


		bool wasDashing = m_Dashing;
		m_Dashing = m_DashReady && dash && static_cast<double>(evnt->key != SCAN_CODES::SPACE);
		if (m_Dashing)
		{
			m_TurningTimer = m_TurningInterval;
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
			if (m_UpdateDashId == -1)
			{
				m_UpdateDashId = specificUpdates.size();
				specificUpdates.push_back(&PlayerInputComponent::updateDash);
			}

			double angle = std::atan2(m_pTransform->GetInvDir() * vel.x, m_pTransform->GetInvDir() * vel.z);
			m_pCC->SetRotation({ 0.0, 1.0, 0.0 }, angle);
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

	// If we press a key, make the player turn towards the direction she is moving
	if (evnt->key != SCAN_CODES::SPACE)
	{
		m_CameraRotating = false;
	}
}

void component::PlayerInputComponent::rotate(MouseMovement* evnt)
{
	// Rotate camera
	// Mouse movement
	int x = evnt->x, y = evnt->y;

	// Determine how much to rotate in radians
	float sensitivityX = stof(Option::GetInstance().GetVariable("f_sensitivityX"));
	float sensitivityY = stof(Option::GetInstance().GetVariable("f_sensitivityY"));

	// rotateX/Y determines how much yaw/pitch changes
	const float constScale = 1 / 1300.0;
	float rotateX = (static_cast<float>(x) * sensitivityX * constScale);
	float rotateY = -(static_cast<float>(y) * sensitivityY * constScale);
	
	// Update Yaw and Pitch
	m_Yaw = m_Yaw - rotateX;
	m_Pitch = m_Pitch - rotateY;

	// Constraints so that you can not look directly under the character
	const float yaw_constraint_offset = 0.01f;
	
	float upper = (PI - yaw_constraint_offset) / 2;
	float lower = -(PI - yaw_constraint_offset) / 2;
	// Clamp between upper and lower
	m_Pitch = max(min(m_Pitch, upper), lower);

	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		m_CameraRotating = true;

		Shop* shop = Player::GetInstance().GetShop();
		//Check if in air. If not, change movement direction to match up with camera direction
		if (m_pCC->CastRay({ 0.0, -1.0, 0.0 }, m_pCC->GetDistanceToBottom() + m_Elevation * 0.75) != -1 && !m_Dashing && shop->IsShop2DGUIDisplaying() == false)
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

			float speed = m_pTransform->GetVelocity();
			if ((std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON) && (m_Attacking || m_TurnToCamera))
			{
				// Check if the player is moving in the direction she is turned. If not, lower the movement speed
				float3 playerDir = m_pTransform->GetForwardFloat3();
				float3 moveDir = { move.x, 0.0, move.z };
				moveDir.normalize();
				playerDir.normalize();

				float moveDif = EngineMath::convertToDegrees(moveDir.angle(playerDir));

				speed *= (1.0f - (moveDif / (180.0f / SLOWDOWN_FACTOR)));
			}

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
		if (m_UpdateShootId == -1)
		{
			m_UpdateShootId = specificUpdates.size();
			specificUpdates.push_back(&PlayerInputComponent::updateShoot);
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
			0.0,
			forward.z * move.z + right.z * move.x
		};
		move.normalize();

		float speed = m_pTransform->GetVelocity();
		if ((std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON) && (m_Attacking || m_TurnToCamera))
		{
			// Check if the player is moving in the direction she is turned. If not, lower the movement speed
			float3 playerDir = m_pTransform->GetForwardFloat3();
			float3 moveDir = { move.x, 0.0, move.z };
			moveDir.normalize();
			playerDir.normalize();

			float moveDif = EngineMath::convertToDegrees(moveDir.angle(playerDir));

			speed *= (1.0f - (moveDif / (180.0f / SLOWDOWN_FACTOR)));
		}

		vel = {
			move.x * speed,
			velY,
			move.z * speed
		};

		m_pCC->SetVelVector(vel.x, vel.y, vel.z);
		m_Dashing = false;

		if (m_UpdateDashId != -1)
		{
			specificUpdates.erase(specificUpdates.begin() + m_UpdateDashId);
			m_UpdateShootId -= static_cast<int>(m_UpdateDashId < m_UpdateShootId);
			m_UpdateDashId = -1;
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

		float speed = m_pTransform->GetVelocity();
		if ((std::abs(move.x) > EPSILON || std::abs(move.z) > EPSILON) && (m_Attacking || m_TurnToCamera))
		{
			// Check if the player is moving in the direction she is turned. If not, lower the movement speed
			float3 playerDir = m_pTransform->GetForwardFloat3();
			float3 moveDir = { move.x, 0.0, move.z };
			moveDir.normalize();
			playerDir.normalize();

			float moveDif = EngineMath::convertToDegrees(moveDir.angle(playerDir));

			speed *= (1.0f - (moveDif / (180.0f / SLOWDOWN_FACTOR)));
		}

		double3 vel = {
			move.x * speed,
			0.0,
			move.z * speed
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
	else
	{
		if (m_UpdateShootId != -1)
		{
			specificUpdates.erase(specificUpdates.begin() + m_UpdateShootId);
			m_UpdateDashId -= static_cast<int>(m_UpdateShootId < m_UpdateDashId);
			m_UpdateShootId = -1;
		}
	}
}
