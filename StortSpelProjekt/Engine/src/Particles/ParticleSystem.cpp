#include "stdafx.h"
#include "ParticleSystem.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DX12Tasks/DX12Task.h"
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
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(double dt)
{
	if (effect != nullptr)
	{
		//for(ParticleEffect& particleEffect : )
		effect->Update(dt);
		effect->updateResourceData();
	}
}

void ParticleSystem::SetParticleEffect(ParticleEffect* effect)
{
	this->effect = effect;
}

void ParticleSystem::OnResetScene()
{
}
