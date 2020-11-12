#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100
#define PARTICLESYSTEM_RENDER_DEAD_PARTICLES 0;

#include <vector>
#include "structs.h"

#include "Particle.h"
#include "../Misc/EngineRand.h"

class DescriptorHeap;
class Resource;
class ShaderResourceView;
class UnorderedAccessView;
class Texture2DGUI;
class Mesh;
namespace component
{
	class ParticleEmitterComponent;
}


struct RandomParameter
{
	float2 interval = { 0.0f, 0.0f }; // default should be 0,0 rand does not work with 0,0
	//rand distribution
};

struct RandomParameter3
{
	union
	{
		RandomParameter params[3];
		struct { RandomParameter x; RandomParameter y; RandomParameter z; };
		struct { RandomParameter r; RandomParameter g; RandomParameter b; };
	};
};

struct ParticleEffectSettings
{
	unsigned int particleCount = PARTICLE_EFFECT_DEFAULT_SIZE;
	float spawnInterval = 0.1;

	// Default Particle Settings
	ParticleAttributes startValues;

	RandomParameter3 randPosition;
	RandomParameter3 randVelocity;
	RandomParameter randSize;
	RandomParameter randRotation;
	RandomParameter randRotationSpeed;
	RandomParameter randLifetime;
};

class ParticleEffect
{
public:
	ParticleEffect(component::ParticleEmitterComponent* pec, DescriptorHeap* descriptorHeap, Texture2DGUI* texture, ParticleEffectSettings* settings);
	~ParticleEffect();

	void Update(double dt);

	Texture2DGUI* GetTexture() const;

private:
	friend class ParticleSystem;
	friend class ParticleRenderTask;
	friend class Renderer;

	static EngineRand rand;

	component::ParticleEmitterComponent* m_pComponentParent = nullptr;
	Texture2DGUI* m_pTexture = nullptr;
	std::vector<Particle> m_Particles;
	std::vector<PARTICLE_DATA> m_ParticlesData;
	unsigned int m_ParticleIndex = 0;
	float m_TimeSinceSpawn = 0;

	ParticleEffectSettings m_Settings = {};

	// Dx12

	// float3, float (pos3, size)
	Resource* m_pUploadResource = nullptr;
	// float3, float (pos3, size)
	Resource* m_pDefaultResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;


	bool spawnParticle();

	void init(DescriptorHeap* descriptorHeap);
	
	void initParticle(Particle& particle);
	void randomizePosition(Particle& particle);
	void randomizeVelocity(Particle& particle);
	void randomizeSize(Particle& particle);
	void randomizeRotation(Particle& particle);
	void randomizeRotationSpeed(Particle& particle);
	void randomizeLifetime(Particle& particle);

	void updateResourceData(float3 cameraPos);
};

#endif
