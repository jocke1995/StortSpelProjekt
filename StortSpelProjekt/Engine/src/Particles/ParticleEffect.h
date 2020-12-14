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

struct ByLifetimeParameter4
{
	float4 end = { 0, 0, 0, 0 };	// Value the particle dies with
};

struct ByLifetimeParameter3
{
	float3 end = { 0, 0, 0 };		// Value the particle dies with
};

struct ByLifetimeParameter2
{
	float2 end = { 0, 0 };			// Value the particle dies with
};

struct ByLifetimeParameter
{
	float end = 0;					// Value the particle dies with
};

struct ParticleEffectSettings
{
	Texture2DGUI* texture = nullptr; // Particles texture.
	unsigned int maxParticleCount = PARTICLE_EFFECT_DEFAULT_SIZE; // Will wait until particle is dead before spawning next
	float spawnInterval = 0.1; // seconds per spawn
	bool isLooping = true; // Sets spawning = false after maxParticleCount particles has been created

	ParticleAttributes startValues; // Start values when particles spawn
	ParticleAttributes endValues; // End values when particles die

	// These get added to the start values when spawning
	// Note that these do not change the interpolation of the start -> end values.
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
	
	// Particle effect data
	ParticleEffectSettings m_Settings = {};
	float m_SizeChangePerFrame = 0;
	float4 m_ColorChangePerFrame = { 0,0,0,0 };

	// Behavior data
	unsigned int m_ParticleIndex = 0;
	double m_TimeSinceSpawn = 0;
	bool m_IsSpawning = false;

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
	void clampFloat(float* floatToClamp, float upper, float lower) const;

	void updateResourceData(float3 cameraPos);
};

#endif
