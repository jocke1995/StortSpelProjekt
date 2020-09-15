#include "stdafx.h"
#include "Audio3DListenerComponent.h"
#include "../AudioEngine/AudioEngine.h"

component::Audio3DListenerComponent::Audio3DListenerComponent(Entity* parent) : Component(parent)
{
	m_Listener = {};
}

component::Audio3DListenerComponent::~Audio3DListenerComponent()
{
}

void component::Audio3DListenerComponent::Update(double dt)
{
}

void component::Audio3DListenerComponent::UpdatePosition()
{
	// get parent entity and look for transform and camera components and get their position/orientation to update m_Listener
}
