#include "stdafx.h"
#include "ParticleEffect.h"

EngineRand ParticleEffect::rand = {};

ParticleEffect::ParticleEffect()
{
	m_ParticleCount = 5;
	init();
}

ParticleEffect::ParticleEffect(unsigned int particleCount)
{
	m_ParticleCount = particleCount;
	
	init();
}

ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::Update(double dt)
{
	m_TimeSinceSpawn += dt;
	
	// spawn particles
	if (m_TimeSinceSpawn >= m_SpawnInterval)
	{
		spawnParticle();

		m_TimeSinceSpawn = 0;
	}

	// Update all particles
	for (Particle& particle : m_Particles)
	{
		// Only update if alive
		if (particle.IsAlive())
		{
			particle.Update(dt);
		}
	}
}

void ParticleEffect::spawnParticle()
{
	// m_ParticleIndex is always at the oldest particle first
	Particle& particle = m_Particles.at(m_ParticleIndex);

	// If particle is alive, don't spawn
	if (particle.IsAlive())
	{
		return;
	}

	// Update ParticleIndex
	m_ParticleIndex = ++m_ParticleIndex % m_ParticleCount;

	// "Spawn"
	initParticle(particle);
}

void ParticleEffect::init()
{
	m_Particles.reserve(m_ParticleCount);

	m_Particles = std::vector<Particle>(m_ParticleCount);
}

void ParticleEffect::initParticle(Particle& particle)
{
	// Todo, later get these from some kind of preset
	particle.initDefaultValues();

	randomizePosition(particle);
	randomizeVelocity(particle);
	randomizeSize(particle);
	randomizeRotation(particle);
	randomizeLifetime(particle);
}

void ParticleEffect::randomizePosition(Particle& particle)
{
	float x = rand.Randf(-1, 1);
	float y = rand.Randf(-1, 1);
	
	particle.m_Position.x = x;
	particle.m_Position.y = y;
}

void ParticleEffect::randomizeVelocity(Particle& particle)
{
}

void ParticleEffect::randomizeSize(Particle& particle)
{
}

void ParticleEffect::randomizeRotation(Particle& particle)
{
}

void ParticleEffect::randomizeLifetime(Particle& particle)
{
	particle.m_Lifetime = 10;
}
