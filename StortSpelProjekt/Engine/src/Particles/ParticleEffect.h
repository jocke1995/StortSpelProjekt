#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#define PARTICLE_EFFECT_DEFAULT_SIZE 100
#define PARTICLESYSTEM_RENDER_DEAD_PARTICLES false;

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

class Entity;

struct RandomParameter
{
	float intervalLower = 0.0f;
	float intervalUpper = 0.0f;
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
	// TODO: add distribution choice
};

struct ParticleEffectSettings
{
	Texture2DGUI* texture = nullptr;
	unsigned int maxParticleCount = PARTICLE_EFFECT_DEFAULT_SIZE; // Will wait until particle is dead before spawning next
	float spawnInterval = 0.1; // seconds per spawn
	bool isLooping = true; // Sets spawning = false after maxParticleCount particles has been created

	// Default Particle Settings
	ParticleAttributes startValues;

	ByLifetimeParameter sizeByLifetime;
	ByLifetimeParameter colorByLifetime;

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
	ParticleEffect(Entity* parent, ParticleEffectSettings* settings);
	~ParticleEffect();

	void Update(double dt);

	void SetIsSpawning(bool value);
	void Clear();

	Texture2DGUI* GetTexture() const;

private:
	friend class ParticleSystem;
	friend class ParticleRenderTask;
	friend class Renderer;

	static EngineRand rand;

	Entity* m_pEntity = nullptr;
	Texture2DGUI* m_pTexture = nullptr;
	std::vector<Particle> m_Particles;
	std::vector<PARTICLE_DATA> m_ParticlesData;
	unsigned int m_ParticleIndex = 0;
	double m_TimeSinceSpawn = 0;
	bool m_IsSpawning = false;

	ParticleEffectSettings m_Settings = {};

	// DX12
	Resource* m_pUploadResource = nullptr;
	Resource* m_pDefaultResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;

	bool isTimeToSpawnParticles() const;
	bool spawnParticle();

	void init();
	
	void initParticle(Particle& particle);

	float3 randomizeParameter(RandomParameter3& param);
	float randomizeParameter(RandomParameter& param);

	float randomizeFloat(float lower, float upper) const;

	void updateResourceData(float3 cameraPos);
};

#endif
