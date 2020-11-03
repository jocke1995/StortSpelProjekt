#include "stdafx.h"
#include "ParticleEffect.h"

ParticleEffect::ParticleEffect()
{
	m_Particles.reserve(PARTICLE_EFFECT_DEFAULT_SIZE);
}

ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::Update(double dt)
{
}

void ParticleEffect::initParticles()
{
	m_Particles = std::vector<Particle>(100);
}
