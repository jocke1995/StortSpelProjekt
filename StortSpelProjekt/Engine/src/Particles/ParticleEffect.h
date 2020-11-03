#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100

#include <vector>

#include "Particle.h"

class ParticleEffect
{
public:
	ParticleEffect();
	~ParticleEffect();

	void Update(double dt);

private:
	std::vector<Particle> m_Particles;

	void initParticles();
};

#endif
