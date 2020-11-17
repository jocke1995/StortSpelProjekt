#include "stdafx.h"
#include "ParticleEmitterComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Renderer.h"

#include "../Misc/AssetLoader.h"

#include "../Particles/ParticleSystem.h"

#include "../ECS/Entity.h"

component::ParticleEmitterComponent::ParticleEmitterComponent(Entity* parent, Texture2DGUI* texture, ParticleEffectSettings* settings, bool playOnInit)
	:Component(parent), m_PlayOnInit(playOnInit), m_ParticleEffect(parent, Renderer::GetInstance().getCBVSRVUAVdHeap(), texture, settings)
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
	if (m_PlayOnInit)
	{
		Play();
	}
	
	Renderer& renderer = Renderer::GetInstance();
	renderer.InitParticleEmitterComponent(this);
	ParticleSystem::GetInstance().ActivateParticleEffect(&m_ParticleEffect);
}

void component::ParticleEmitterComponent::OnUnInitScene()
{
	Stop();
	Renderer& renderer = Renderer::GetInstance();
	renderer.UnInitParticleEmitterComponent(this);
	ParticleSystem::GetInstance().DeactivateParticleEffect(&m_ParticleEffect);
}

void component::ParticleEmitterComponent::Play()
{
	if (m_IsPlaying)
	{
		return;
	}

	m_ParticleEffect.SetIsSpawning(true);
	
	m_IsPlaying = true;
}

void component::ParticleEmitterComponent::Stop()
{
	if (!m_IsPlaying)
	{
		return;
	}

	m_ParticleEffect.SetIsSpawning(false);

	m_IsPlaying = false;
}

bool component::ParticleEmitterComponent::IsPlaying() const
{
	return m_IsPlaying;
}

const ParticleEffect* component::ParticleEmitterComponent::GetParticleEffect() const
{
	return &m_ParticleEffect;
}