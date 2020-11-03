#include "stdafx.h"
#include "ParticleSystem.h"

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
	effect = new ParticleEffect();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(double dt)
{
}

void ParticleSystem::OnResetScene()
{
}
