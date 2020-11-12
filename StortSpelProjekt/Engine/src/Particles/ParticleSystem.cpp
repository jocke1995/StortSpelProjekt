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

	// Add to COPY_PER_FRAME
	Renderer& renderer = Renderer::GetInstance();
	
	const void* pData = effect->m_ParticlesData.data();
	renderer.submitToCpft(&std::tuple(effect->m_pUploadResource, effect->m_pDefaultResource, pData));
}

void ParticleSystem::DeactivateParticleEffect(ParticleEffect* effect)
{
	m_ActiveParticleEffects.erase(effect);

	// remove from COPY_PER_FRAME
	Renderer& renderer = Renderer::GetInstance();

	const void* pData = effect->m_ParticlesData.data();
	renderer.clearSpecificCpft(effect->m_pUploadResource);
}
