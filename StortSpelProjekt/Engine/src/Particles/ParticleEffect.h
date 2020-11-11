#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100
#define PARTICLESYSTEM_RENDER_DEAD_PARTICLES 0;

#include <vector>

#include "Particle.h"
#include "../Misc/EngineRand.h"

class DescriptorHeap;
class Resource;
class ShaderResourceView;
class UnorderedAccessView;
class Texture2DGUI;
class Mesh;

class ParticleEffect
{
public:
	ParticleEffect(std::wstring name, DescriptorHeap* descriptorHeap, Texture2DGUI* texture, unsigned int particleCount = PARTICLE_EFFECT_DEFAULT_SIZE);
	~ParticleEffect();

	void Update(double dt);

	const std::wstring& GetName() const;

	Texture2DGUI* GetTexture() const;

private:
	friend class ParticleSystem;
	friend class BillboardComputeTask;
	friend class ParticleRenderTask;

	static EngineRand rand;

	struct PARTICLE_DATA
	{
		float3 position;
		float size;
	};

	std::wstring m_Name = L"unnamed_particleEffect";
	Texture2DGUI* m_pTexture = nullptr;
	std::vector<Particle> m_Particles;
	std::vector<PARTICLE_DATA> m_ParticlesData;
	unsigned int m_ParticleIndex = 0;
	unsigned int m_ParticleCount = PARTICLE_EFFECT_DEFAULT_SIZE;

	float m_TimeSinceSpawn = 0;
	float m_SpawnInterval = 0.5;

	// Dx12

	// float3, float (pos3, size)
	Resource* m_pUploadResource = nullptr;
	// float3, float (pos3, size)
	Resource* m_pDefaultResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;


	void spawnParticle();

	void init(std::wstring name, DescriptorHeap* descriptorHeap, Texture2DGUI* texture);
	
	void initParticle(Particle& particle);
	void randomizePosition(Particle& particle);
	void randomizeVelocity(Particle& particle);
	void randomizeSize(Particle& particle);
	void randomizeRotation(Particle& particle);
	void randomizeLifetime(Particle& particle);

	void updateResourceData();
};

#endif
