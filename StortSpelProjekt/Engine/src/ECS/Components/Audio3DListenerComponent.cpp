#include "stdafx.h"
#include "Audio3DListenerComponent.h"
#include "../AudioEngine/AudioEngine.h"
#include "../Renderer/Transform.h"
#include "TransformComponent.h"
#include "../Entity.h"
#include "../Renderer/BaseCamera.h"

component::Audio3DListenerComponent::Audio3DListenerComponent(Entity* parent) : Component(parent)
{
	//m_Listener = {};
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
	//Log::Print("x: %f, y: %f, z:%f\n", m_pTransform->GetPositionXMFLOAT3().x, m_pTransform->GetPositionXMFLOAT3().y, m_pTransform->GetPositionXMFLOAT3().z);
	AudioEngine::GetInstance().SetListener(m_pCamera->GetDirection(), m_pCamera->GetUpVector(), m_pTransform->GetPositionXMFLOAT3());
}
