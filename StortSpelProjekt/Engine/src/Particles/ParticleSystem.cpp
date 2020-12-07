#include "stdafx.h"
#include "ParticleSystem.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DX12Tasks/DX12Task.h"
#include "../Renderer/DescriptorHeap.h"

// Component
#include "../ECS/Components/ParticleEmitterComponent.h"

// Particle Stuff
#include "ParticleEffect.h"

// For sort
#include "../Renderer/Camera/BaseCamera.h"

ParticleSystem& ParticleSystem::GetInstance()
{
	static ParticleSystem instance;
	return instance;
}

ParticleSystem::ParticleSystem()
{
	Renderer& renderer = Renderer::GetInstance();
	m_ActiveParticleEffects.reserve(50);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(double dt)
{
	Renderer& renderer = Renderer::GetInstance();
	for(ParticleEffect* effect : m_ActiveParticleEffects)
	{
		//for(ParticleEffect& particleEffect : )
		effect->Update(dt);
		effect->updateResourceData(renderer.m_pScenePrimaryCamera->GetPositionFloat3());
	}
}

void ParticleSystem::ActivateParticleEffect(ParticleEffect* effect)
{
	m_ActiveParticleEffects.insert(effect);
	effect->m_TimeSinceSpawn = 0;

	// Add to COPY_PER_FRAME
	Renderer& renderer = Renderer::GetInstance();
	
	auto tempData = std::tuple(effect->m_pUploadResource, effect->m_pDefaultResource);
	renderer.submitToCpft(&tempData);
}

void ParticleSystem::DeactivateParticleEffect(ParticleEffect* effect)
{
	m_ActiveParticleEffects.erase(effect);

	// remove from COPY_PER_FRAME
	Renderer& renderer = Renderer::GetInstance();
	renderer.clearSpecificCpft(effect->m_pUploadResource);
}
