#include "InputComponent.h"
#include "../Events/EventBus.h"
#include "../Entity.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/Transform.h"

component::InputComponent::InputComponent(Entity* parent, unsigned int camFlags)
	:Component(parent)
{	
	EventBus::GetInstance().Subscribe(this, &InputComponent::toggleCameraLock);
	EventBus::GetInstance().Subscribe(this, &InputComponent::zoom);
	EventBus::GetInstance().Subscribe(this, &InputComponent::rotate);
	EventBus::GetInstance().Subscribe(this, &InputComponent::move);

	m_CameraFlags = camFlags;

	m_Zoom = 3.0f;

	m_Pitch = 1.0f;

	m_Yaw = 1.0f;

	m_CameraDistance = sqrt(m_Zoom * 4 * m_Zoom * 4 + m_Zoom * m_Zoom);
}

component::InputComponent::~InputComponent()
{
}

void component::InputComponent::Update(double dt)
{
	PerspectiveCamera* cam = static_cast<PerspectiveCamera*>(m_pParent->GetComponent<component::CameraComponent>()->GetCamera());
	Transform* trans = static_cast<Transform*>(m_pParent->GetComponent<component::TransformComponent>()->GetTransform());
	// Lock camera to player
	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
		float3 playerPosition = tc->GetPositionFloat3();

		DirectX::XMMATRIX rotMat = tc->GetRotMatrix();
		DirectX::XMFLOAT3 forward, right;
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&right, rotMat.r[0]);

		float zoomBack = sqrt(m_CameraDistance * m_CameraDistance - (m_Zoom * m_Pitch) * (m_Zoom * m_Pitch));

		float3 cameraPosition = { playerPosition.x - (zoomBack * forward.x), playerPosition.y + m_Zoom * m_Pitch, playerPosition.z - (zoomBack * forward.z) };

		cam->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
		float directionX = playerPosition.x - cameraPosition.x;
		float directionY = playerPosition.y + 3.0f - cameraPosition.y;
		float directionZ = playerPosition.z - cameraPosition.z;
		cam->SetDirection(directionX, directionY, directionZ);
	}
	else
	{
		cam->SetDirection(cos(m_Yaw), m_Pitch * -2, sin(m_Yaw));
	}
}

void component::InputComponent::toggleCameraLock(ModifierInput* evnt)
{
	if (evnt->key == SCAN_CODES::LEFT_CTRL && evnt->pressed)
	{
		// XOR operator. If m_CameraFlags contains the flag, remove it. Otherwise add it to m_CameraFlags.
		m_CameraFlags ^= CAMERA_FLAGS::USE_PLAYER_POSITION;
		static_cast<PerspectiveCamera*>(m_pParent->GetComponent<component::CameraComponent>()->GetCamera())->SetMovement(0.0f, 0.0f, 0.0f);
		static_cast<Transform*>(m_pParent->GetComponent<component::TransformComponent>()->GetTransform())->SetMovement(0.0f, 0.0f, 0.0f);
		m_Yaw = sqrt(2);
		m_Pitch = 0.0f;
	}
}

void component::InputComponent::zoom(MouseScroll* evnt)
{
	m_Zoom = std::max(m_Zoom - static_cast<float>(evnt->scroll) / 4, 1.5f);
	m_CameraDistance = sqrt(m_Zoom * 4 * m_Zoom * 4 + m_Zoom * m_Zoom);
}

void component::InputComponent::move(MovementInput* evnt)
{

	Transform* trans = static_cast<Transform*>(m_pParent->GetComponent<component::TransformComponent>()->GetTransform());
	PerspectiveCamera* cam = static_cast<PerspectiveCamera*>(m_pParent->GetComponent<component::CameraComponent>()->GetCamera());

	// Check if the key has just been pressed or jsut been released and convert to a float. Multiply by two and subtract one to get 1 for true and -1 for false. If
	// the key has been pressed, the player should start moving in the direction specified by the key -- hence the value 1. If the key has been released, the player's
	// movement should be negated in the direction specified by the key -- hence the value -1
	float pressed = (static_cast<float>(evnt->pressed) * 2 - 1);

	// Find out which key has been pressed. Convert to float to get the value 1 if the key pressed should move the player in the positive
	// direction and the value -1 if the key pressed should move the player in the negative direction
	float moveRight = (static_cast<float>(evnt->key == SCAN_CODES::D) - static_cast<float>(evnt->key == SCAN_CODES::A)) * pressed;
	float moveUp = (static_cast<float>(evnt->key == SCAN_CODES::Q) - static_cast<float>(evnt->key == SCAN_CODES::E)) * pressed;
	float moveForward = (static_cast<float>(evnt->key == SCAN_CODES::W) - static_cast<float>(evnt->key == SCAN_CODES::S)) * pressed;

	// Get the rotation matrix to determine in which direction to move
	DirectX::XMMATRIX rotMat = trans->GetRotMatrix();
	DirectX::XMFLOAT3 forward, right;
	DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
	DirectX::XMStoreFloat3(&right, rotMat.r[0]);

	float moveX = forward.x * moveForward + right.x * moveRight;
	float moveY = moveUp;
	float moveZ = forward.z * moveForward + right.z * moveRight;

	(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? trans->UpdateMovement(moveX, moveY, moveZ) : cam->UpdateMovement(-moveRight, moveUp, moveForward);

	// If all buttons are released, reset the movement
	if (!(Input::GetInstance().GetKeyState(SCAN_CODES::W)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::A)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::S)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::D)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::Q)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::E)))
	{
		(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? trans->SetMovement(0.0f, 0.0f, 0.0f) : cam->SetMovement(0.0f, 0.0f, 0.0f);;
	}


}

void component::InputComponent::rotate(MouseMovement* evnt)
{
	CameraComponent* cam = static_cast<CameraComponent*>(m_pParent->GetComponent<component::CameraComponent>());
	Transform* trans = static_cast<Transform*>(m_pParent->GetComponent<component::TransformComponent>()->GetTransform());

	// Rotate camera
	// Mouse movement
	int x = evnt->x, y = evnt->y;

	// Determine how much to rotate in radians
	float rotateY = (static_cast<float>(y) / 600.0) * 3.1415;
	float rotateX = -(static_cast<float>(x) / 800.0) * 3.1415;

	m_Pitch = std::max(std::min(m_Pitch + rotateY, 3.0f), -3.0f);
	m_Yaw = m_Yaw + rotateX;

	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		// Rotate transform
		// Determine how much to rotate in radians
		rotateX = (static_cast<float>(x) / 400.0) * 3.1415;

		// Get rotation to determine current rotation angle
		DirectX::XMMATRIX rotMat = trans->GetRotMatrix();
		DirectX::XMFLOAT3 forward, right;
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&right, rotMat.r[0]);

		float angle = std::atan2(forward.x, forward.z);

		// Set the new rotation
		trans->SetRotationY(angle + rotateX);

		// Get new direction
		rotMat = trans->GetRotMatrix();
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&right, rotMat.r[0]);

		// Determine if player is currently moving, if yes, update movement direction
		int isMovingZ = static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::S));
		int isMovingX = static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::A));

		trans->SetMovement(forward.x * isMovingZ + right.x * isMovingX, trans->GetMovement().y, forward.z * isMovingZ + right.z * isMovingX);
	}
}
