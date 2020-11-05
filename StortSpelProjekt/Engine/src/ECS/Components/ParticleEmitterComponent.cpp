#include "stdafx.h"
#include "ParticleEmitterComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Renderer.h"

#include "../Misc/AssetLoader.h"

component::ParticleEmitterComponent::ParticleEmitterComponent(Entity* parent)
	:Component(parent)
{
	
}

component::ParticleEmitterComponent::~ParticleEmitterComponent()
{
}

void component::ParticleEmitterComponent::RenderUpdate(double dt)
{
}

void component::ParticleEmitterComponent::OnInitScene()
{
}

void component::ParticleEmitterComponent::OnUnInitScene()
{
}

void component::ParticleEmitterComponent::SetTexture(Texture2DGUI* texture)
{
}

Texture2DGUI* component::ParticleEmitterComponent::GetTexture() const
{
	return nullptr;
}
