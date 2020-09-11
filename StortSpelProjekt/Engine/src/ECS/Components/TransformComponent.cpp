#include "stdafx.h"
#include "TransformComponent.h"
#include "../../Renderer/Transform.h"
#include "../ECS/Entity.h"

// TEMP FOR TESTING CAMERA
#include "../Input/Input.h"
#include "../Renderer/BaseCamera.h"

namespace component
{
	TransformComponent::TransformComponent(Entity* parent)
		:Component(parent)
	{
		m_pTransform = new Transform();
	}

	TransformComponent::~TransformComponent()
	{
		delete m_pTransform;
	}

	void TransformComponent::Update(double dt)
	{
		// TEMPORARY FOR TESTING CAMERA
		if (!std::strcmp(m_pParent->GetName().c_str(), "player"))
		{
			if (Input::GetInstance().GetKeyState(SCAN_CODES::W))
			{
				DirectX::XMFLOAT3 pos = m_pTransform->GetPositionXMFLOAT3();
				pos.z += 10 * dt;
				m_pTransform->SetPosition(pos);
			}
			if (Input::GetInstance().GetKeyState(SCAN_CODES::A))
			{
				DirectX::XMFLOAT3 pos = m_pTransform->GetPositionXMFLOAT3();
				pos.x -= 10 * dt;
				m_pTransform->SetPosition(pos);
			}
			if (Input::GetInstance().GetKeyState(SCAN_CODES::S))
			{
				DirectX::XMFLOAT3 pos = m_pTransform->GetPositionXMFLOAT3();
				pos.z -= 10 * dt;
				m_pTransform->SetPosition(pos);
			}
			if (Input::GetInstance().GetKeyState(SCAN_CODES::D))
			{
				DirectX::XMFLOAT3 pos = m_pTransform->GetPositionXMFLOAT3();
				pos.x += 10 * dt;
				m_pTransform->SetPosition(pos);
			}
			if (Input::GetInstance().GetKeyState(SCAN_CODES::Q))
			{
				DirectX::XMFLOAT3 pos = m_pTransform->GetPositionXMFLOAT3();
				pos.y += 10 * dt;
				m_pTransform->SetPosition(pos);
			}
			if (Input::GetInstance().GetKeyState(SCAN_CODES::E))
			{
				DirectX::XMFLOAT3 pos = m_pTransform->GetPositionXMFLOAT3();
				pos.y -= 10 * dt;
				m_pTransform->SetPosition(pos);
			}
		}

		m_pTransform->UpdateWorldMatrix();
	}

	Transform* TransformComponent::GetTransform() const
	{
		return m_pTransform;
	}
}
