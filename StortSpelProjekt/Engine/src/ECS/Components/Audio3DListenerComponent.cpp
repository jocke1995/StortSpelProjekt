#include "stdafx.h"
#include "Audio3DListenerComponent.h"
#include "../AudioEngine/AudioEngine.h"
#include "../Renderer/Transform.h"
#include "TransformComponent.h"
#include "../Entity.h"
#include "../Renderer/BaseCamera.h"

component::Audio3DListenerComponent::Audio3DListenerComponent(Entity* parent) : Component(parent)
{
	//m_Listener = { };
}

component::Audio3DListenerComponent::~Audio3DListenerComponent()
{
}

void component::Audio3DListenerComponent::Update(double dt)
{
}

void component::Audio3DListenerComponent::UpdatePosition()
{
	// get parent entity and look for transform and camera components and get their orientFront, orientTop and position to update m_Listener
	m_pCamera = m_pParent->GetComponent<CameraComponent>()->GetCamera();
	m_pTransform = m_pParent->GetComponent<TransformComponent>()->GetTransform();

	DirectX::XMFLOAT3 zDirection(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 yDirection(0.0f, 1.0f, 0.0f);

	//Log::Print("x: %f, y: %f, z:%f\n", m_pTransform->GetPositionXMFLOAT3().x, m_pTransform->GetPositionXMFLOAT3().y, m_pTransform->GetPositionXMFLOAT3().z);
	AudioEngine::GetInstance().SetListener(zDirection, yDirection, m_pTransform->GetPositionXMFLOAT3());

	//m_Listener.OrientFront = m_pCamera->GetDirection();
	//m_Listener.OrientTop = m_pCamera->GetUpVector();
	////// left handed to right handed coordinates z-change
	////DirectX::XMFLOAT3 rhz;
	////rhz.x = position.x;
	////rhz.y = position.y;
	////rhz.z = -position.z;
	////m_Listener.Position = rhz;
	//m_Listener.Position = m_pTransform->GetPositionXMFLOAT3();
	
	//Log::Print("x: %f, y: %f, z:%f\n", m_Listener.Position.x, m_Listener.Position.y, m_Listener.Position.z);
}

//X3DAUDIO_LISTENER* component::Audio3DListenerComponent::GetListener()
//{
//	return &m_Listener;
//}
