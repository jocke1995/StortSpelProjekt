#include "stdafx.h"
#include "ParticleSystem.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DX12Tasks/DX12Task.h"
#include "../Renderer/DX12Tasks/BillboardComputeTask.h"
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

void ParticleSystem::uploadParticleEffectsToGPU()
{
	// todo, for each effect uploadResourceData

	effect->updateResourceData();
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

		std::vector<ParticleEffect*> effects = { effect };

		// Tell renderer to billboard the particles
		BillboardComputeTask* billboardTask = static_cast<BillboardComputeTask*>(Renderer::GetInstance().m_ComputeTasks[COMPUTE_TASK_TYPE::BILLBOARD]);
		billboardTask->SetParticleEffects(&effects);
	}
}

void ParticleSystem::SetParticleEffect(ParticleEffect* effect)
{
	this->effect = effect;
}

void ParticleSystem::OnResetScene()
{
}
