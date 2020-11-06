#include "stdafx.h"
#include "ParticleEmitterComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Renderer.h"

#include "../Misc/AssetLoader.h"

#include "../Particles/ParticleSystem.h"
#include "../Particles/ParticleEffect.h"

#include "../ECS/Entity.h"

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
	if (m_pParticleEffect != nullptr)
	{
		ParticleSystem::GetInstance().SetParticleEffect(m_pParticleEffect);

		Renderer& renderer = Renderer::GetInstance();
		renderer.InitParticleEmitterComponent(this);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "ParticleEffect not set on component @Entity: %s\n", m_pParent->GetName().c_str());
	}
}

void component::ParticleEmitterComponent::OnUnInitScene()
{
	// TODO: remove particle effect from particle system


	Renderer& renderer = Renderer::GetInstance();
	renderer.UnitParticleEmitterComponent(this);
}

void component::ParticleEmitterComponent::SetParticleEffect(ParticleEffect* effect)
{
	m_pParticleEffect = effect;
}

Texture2DGUI* component::ParticleEmitterComponent::GetTexture() const
{
	return m_pParticleEffect->GetTexture();
}