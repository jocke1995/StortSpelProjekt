#include "stdafx.h"
#include "ParticleSystem.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DescriptorHeap.h"

// Component
#include "../ECS/Components/ParticleEmitterComponent.h"

// Particle Stuff
#include "ParticleEffect.h"

ParticleSystem& ParticleSystem::GetInstance()
{
	static ParticleSystem instance;
	return instance;
}

ParticleSystem::ParticleSystem()
{
	Renderer& renderer = Renderer::GetInstance();

	effect = new ParticleEffect(renderer.m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
}

void ParticleSystem::uploadParticleEffectsToGPU()
{
	// todo, for each effect uploadResourceData

	effect->updateResourceData();
}

ParticleSystem::~ParticleSystem()
{
	
	if(effect != nullptr)
	delete effect;
}

void ParticleSystem::Update(double dt)
{
	//for(ParticleEffect& particleEffect : )
	effect->Update(dt);
	effect->updateResourceData();
}

void ParticleSystem::OnResetScene()
{
}
