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
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(double dt)
{
	Renderer& renderer = Renderer::GetInstance();
	if (m_pEffect != nullptr)
	{
		//for(ParticleEffect& particleEffect : )
		m_pEffect->Update(dt);
		m_pEffect->updateResourceData(renderer.m_pScenePrimaryCamera->GetPositionFloat3());
	}
}

void ParticleSystem::SetParticleEffect(ParticleEffect* effect)
{
	m_pEffect = effect;
}

void ParticleSystem::OnResetScene()
{
}
