#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100

#include <vector>

#include "Particle.h"
#include "../Misc/EngineRand.h"

class ParticleEffect
{
public:
	ParticleEffect();
	ParticleEffect(unsigned int particleCount);
	~ParticleEffect();

	void Update(double dt);

private:
	static EngineRand rand;

	std::vector<Particle> m_Particles;
	unsigned int m_ParticleIndex = 0;
	unsigned int m_ParticleCount = PARTICLE_EFFECT_DEFAULT_SIZE;

	float m_TimeSinceSpawn = 0;
	float m_SpawnInterval = 0.5;

	void spawnParticle();

	void init();
	
	void initParticle(Particle& particle);
	void randomizePosition(Particle& particle);
	void randomizeVelocity(Particle& particle);
	void randomizeSize(Particle& particle);
	void randomizeRotation(Particle& particle);
	void randomizeLifetime(Particle& particle);
};

#endif
