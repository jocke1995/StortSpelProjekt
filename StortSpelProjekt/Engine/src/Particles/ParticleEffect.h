#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

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
};

#endif
