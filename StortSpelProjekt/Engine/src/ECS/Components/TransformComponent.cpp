#include "stdafx.h"
#include "TransformComponent.h"
#include "../../Renderer/Transform.h"
#include "../ECS/Entity.h"

#include "../Events/EventBus.h"

// TEMP FOR TESTING CAMERA
#include "../Input/Input.h"
#include "../Renderer/BaseCamera.h"

namespace component
{
	TransformComponent::TransformComponent(Entity* parent)
		:Component(parent)
	{
		m_pTransform = new Transform();

		// If parent is the player, subscribe to movement events
		if (!std::strcmp(m_pParent->GetName().c_str(), "player"))
		{
			EventBus::GetInstance().Subscribe(this, &TransformComponent::setMovement);
			EventBus::GetInstance().Subscribe(this, &TransformComponent::setRotation);
		}
	}

	TransformComponent::~TransformComponent()
	{
		delete m_pTransform;
	}

	void TransformComponent::Update(double dt)
	{
		m_pTransform->Move(dt);
		m_pTransform->UpdateWorldMatrix();
	}

	Transform* TransformComponent::GetTransform() const
	{
		return m_pTransform;
	}
	
	void TransformComponent::setMovement(MovementInput* evnt)
	{
		// Find out which key has been pressed
		float moveRight = static_cast<float>(evnt->key == SCAN_CODES::D) * (static_cast<float>(evnt->pressed) * 2 - 1) - static_cast<float>((evnt->key == SCAN_CODES::A) * (static_cast<float>(evnt->pressed) * 2 - 1));
		float moveUp = static_cast<float>(evnt->key == SCAN_CODES::Q) * (static_cast<float>(evnt->pressed) * 2 - 1) - static_cast<float>((evnt->key == SCAN_CODES::E) * (static_cast<float>(evnt->pressed) * 2 - 1));
		float moveForward = static_cast<float>(evnt->key == SCAN_CODES::W) * (static_cast<float>(evnt->pressed) * 2 - 1) - static_cast<float>((evnt->key == SCAN_CODES::S) * (static_cast<float>(evnt->pressed) * 2 - 1));

		// Get the rotation matrix to determine in which direction to move
		DirectX::XMMATRIX rotMat = m_pTransform->GetRotMatrix();
		DirectX::XMFLOAT3 forward, right;
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&right, rotMat.r[0]);

		float moveX = forward.x * moveForward + right.x * moveRight;
		float moveY = moveUp;
		float moveZ = forward.z * moveForward + right.z * moveRight;

		m_pTransform->UpdateMovement(moveX, moveY, moveZ);

		// If all buttons are released, reset the movement
		if (!(Input::GetInstance().GetKeyState(SCAN_CODES::W)) &&
			!(Input::GetInstance().GetKeyState(SCAN_CODES::A)) &&
			!(Input::GetInstance().GetKeyState(SCAN_CODES::S)) &&
			!(Input::GetInstance().GetKeyState(SCAN_CODES::D)) &&
			!(Input::GetInstance().GetKeyState(SCAN_CODES::Q)) &&
			!(Input::GetInstance().GetKeyState(SCAN_CODES::E)))
		{
			m_pTransform->SetMovement(0.0f, 0.0f, 0.0f);
		}
	}
	void TransformComponent::setRotation(MouseMovement* evnt)
	{
		// Mouse movement in x-direction
		int x = evnt->x, y = evnt->x;

		// Determine how much to rotate in radians
		float rotateX = -(static_cast<float>(x) / 400.0) * 3.1415;

		// Get rotation to determine current rotation angle
		DirectX::XMMATRIX rotMat = m_pTransform->GetRotMatrix();
		DirectX::XMFLOAT3 forward, right;
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&right, rotMat.r[0]);

		float angle = std::atan2(forward.x, forward.z);

		// Set the new rotation
		m_pTransform->RotateY(angle + rotateX);

		// Get new direction
		rotMat = m_pTransform->GetRotMatrix();
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&right, rotMat.r[0]);

		// Determine if player is currently moving, if yes, update movement direction
		int isMovingZ = static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::S));
		int isMovingX = static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::A));

		m_pTransform->SetMovement(forward.x * isMovingZ + right.x * isMovingX, m_pTransform->GetMovement().y, forward.z * isMovingZ + right.z * isMovingX);
	}
}
