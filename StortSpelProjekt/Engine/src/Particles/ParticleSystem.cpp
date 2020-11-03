#include "stdafx.h"
#include "ParticleSystem.h"

// Component
#include "../ECS/Components/ParticleEmitterComponent.h"

ParticleSystem& ParticleSystem::GetInstance()
{
	static ParticleSystem instance;
	return instance;
}

ParticleSystem::ParticleSystem()
{

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
