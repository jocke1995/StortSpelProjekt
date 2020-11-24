#include "stdafx.h"
#include "ParticleEmitterComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Renderer.h"

#include "../Misc/AssetLoader.h"

#include "../Particles/ParticleSystem.h"

#include "../ECS/Entity.h"

component::ParticleEmitterComponent::ParticleEmitterComponent(Entity* parent, std::vector<ParticleEffectSettings>* settings, bool playOnInit)
	:Component(parent), m_PlayOnInit(playOnInit)
{
	// Allocate Memory
	m_ParticleEffects.reserve(settings->size());

	// Create the ParticleEffects
	for (unsigned int i = 0; i < settings->size(); i++)
	{
		m_ParticleEffects.push_back(new ParticleEffect(parent, &settings->at(i)));
	}
}

component::ParticleEmitterComponent::ParticleEmitterComponent(Entity* parent, ParticleEffectSettings* settings, bool playOnInit)
	:Component(parent), m_PlayOnInit(playOnInit)
{
	// Create the ParticleEffect
	m_ParticleEffects.push_back(new ParticleEffect(parent, settings));
}

component::ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (unsigned int i = 0; i < m_ParticleEffects.size(); i++)
	{
		delete m_ParticleEffects[i];
	}
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

	for (unsigned int i = 0; i < m_ParticleEffects.size(); i++)
	{
		ParticleSystem::GetInstance().ActivateParticleEffect(m_ParticleEffects.at(i));
	}
}

void component::ParticleEmitterComponent::OnUnInitScene()
{
	Stop();
	Renderer& renderer = Renderer::GetInstance();
	renderer.UnInitParticleEmitterComponent(this);

	for (unsigned int i = 0; i < m_ParticleEffects.size(); i++)
	{
		ParticleSystem::GetInstance().DeactivateParticleEffect(m_ParticleEffects.at(i));
	}
}

void component::ParticleEmitterComponent::Play()
{
	if (m_IsPlaying)
	{
		return;
	}

	for (unsigned int i = 0; i < m_ParticleEffects.size(); i++)
	{
		m_ParticleEffects.at(i)->SetIsSpawning(true);
	}
	
	m_IsPlaying = true;
}

void component::ParticleEmitterComponent::Stop()
{
	if (!m_IsPlaying)
	{
		return;
	}

	for (unsigned int i = 0; i < m_ParticleEffects.size(); i++)
	{
		m_ParticleEffects.at(i)->SetIsSpawning(false);
	}

	m_IsPlaying = false;
}

bool component::ParticleEmitterComponent::IsPlaying() const
{
	return m_IsPlaying;
}

const std::vector<ParticleEffect*>* component::ParticleEmitterComponent::GetParticleEffects() const
{
	return &m_ParticleEffects;
}