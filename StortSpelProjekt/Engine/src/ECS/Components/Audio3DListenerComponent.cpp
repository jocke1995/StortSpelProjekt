#include "stdafx.h"
#include "Audio3DListenerComponent.h"
#include "../AudioEngine/AudioEngine.h"
#include "../Renderer/Transform.h"
#include "../Entity.h"

component::Audio3DListenerComponent::Audio3DListenerComponent(Entity* parent) : Component(parent)
{
	m_Listener = { };
	// set m_pListener in AudioEngine to point to this listener
	AudioEngine::GetInstance().SetListenerPtr(&m_Listener);
}

component::Audio3DListenerComponent::~Audio3DListenerComponent()
{
}

void component::Audio3DListenerComponent::Update(double dt)
{
	// Temporary for sandbox test, UpdateListener will later be explicitly called when needed
	UpdateListener();
}

void component::Audio3DListenerComponent::InitScene(Renderer* renderer)
{
	Log::Print("Audio3DListenerComponent InitScene called!\n");
}

void component::Audio3DListenerComponent::UpdateListener()
{
	// get parent entity and look for transform component and get the forward, up and position vectors to update m_Listener
	m_pTransform = m_pParent->GetComponent<TransformComponent>()->GetTransform();
	DirectX::XMMATRIX rotMat = m_pTransform->GetRotMatrix();
	DirectX::XMFLOAT3 forward, up, position;
	DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
	DirectX::XMStoreFloat3(&up, rotMat.r[1]);
	position = m_pTransform->GetPositionXMFLOAT3();
	SetListener(forward, up, position);
}

void component::Audio3DListenerComponent::SetListener(DirectX::XMFLOAT3 orientFront, DirectX::XMFLOAT3 orientTop, DirectX::XMFLOAT3 position)
{
	m_Listener.OrientFront = orientFront;
	m_Listener.OrientTop = orientTop;
	m_Listener.Position = position;
}

X3DAUDIO_LISTENER* component::Audio3DListenerComponent::GetListener()
{
	return &m_Listener;
}
