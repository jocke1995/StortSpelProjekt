#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100

#include <vector>

#include "Particle.h"
#include "../Misc/EngineRand.h"

class DescriptorHeap;
class Resource;
class ShaderResourceView;
class UnorderedAccessView;
class Texture2DGUI;

class ParticleEffect
{
public:
	ParticleEffect(std::wstring name, DescriptorHeap* descriptorHeap);
	ParticleEffect(std::wstring name, DescriptorHeap* descriptorHeap, unsigned int particleCount);
	~ParticleEffect();

	void Update(double dt);

	const std::wstring& GetName() const;

	Texture2DGUI* GetTexture() const;

private:
	friend class ParticleSystem;
	friend class BillboardComputeTask;

	static EngineRand rand;

	std::wstring m_Name = L"unnamed_particleEffect";
	Texture2DGUI* m_pTexture = nullptr;
	std::vector<Particle> m_Particles;
	unsigned int m_ParticleIndex = 0;
	unsigned int m_ParticleCount = PARTICLE_EFFECT_DEFAULT_SIZE;

	float m_TimeSinceSpawn = 0;
	float m_SpawnInterval = 0.5;

	// Dx12
	Resource* m_pUploadResource = nullptr;
	Resource* m_pUAVResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;
	UnorderedAccessView* m_pUAV = nullptr;

	void spawnParticle();

	void init(std::wstring name, DescriptorHeap* descriptorHeap);
	
	void initParticle(Particle& particle);
	void randomizePosition(Particle& particle);
	void randomizeVelocity(Particle& particle);
	void randomizeSize(Particle& particle);
	void randomizeRotation(Particle& particle);
	void randomizeLifetime(Particle& particle);

	void updateResourceData();
};

#endif
