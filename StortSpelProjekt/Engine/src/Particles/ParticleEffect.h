#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100

#include <vector>

#include "Particle.h"
#include "../Misc/EngineRand.h"

class DescriptorHeap;
class Resource;
class ShaderResourceView;
class Texture2DGUI;

class ParticleEffect
{
public:
	ParticleEffect(DescriptorHeap* descriptorHeap);
	ParticleEffect(DescriptorHeap* descriptorHeap, unsigned int particleCount);
	~ParticleEffect();

	void Update(double dt);

private:
	friend class ParticleSystem;

	static EngineRand rand;

	Texture2DGUI* m_pTexture = nullptr;
	std::vector<Particle> m_Particles;
	unsigned int m_ParticleIndex = 0;
	unsigned int m_ParticleCount = PARTICLE_EFFECT_DEFAULT_SIZE;

	float m_TimeSinceSpawn = 0;
	float m_SpawnInterval = 0.5;

	// Dx12
	Resource* m_pUploadResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;

	void spawnParticle();

	void init(DescriptorHeap* descriptorHeap);
	
	void initParticle(Particle& particle);
	void randomizePosition(Particle& particle);
	void randomizeVelocity(Particle& particle);
	void randomizeSize(Particle& particle);
	void randomizeRotation(Particle& particle);
	void randomizeLifetime(Particle& particle);

	void updateResourceData();
};

#endif
