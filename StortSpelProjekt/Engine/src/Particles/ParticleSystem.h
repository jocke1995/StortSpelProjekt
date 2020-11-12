#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

class ParticleEffect;
class DescriptorHeap;

#include <unordered_set>

namespace component
{
	class ParticleEmitterComponent;
}

class ParticleSystem
{
public:
	static ParticleSystem& GetInstance();
	~ParticleSystem();

	void Update(double dt);

	void ActivateParticleEffect(ParticleEffect* effect);
	void DeactivateParticleEffect(ParticleEffect* effect);

private:
	ParticleSystem();

	// Temp code todo: vector of effectComponents. component::init() pushes in.
	std::unordered_set<ParticleEffect*> m_ActiveParticleEffects;

};

#endif
