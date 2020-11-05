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
	delete effect;
}

void ParticleSystem::Update(double dt)
{
	effect->Update(dt);
}

void ParticleSystem::OnResetScene()
{
}
