#ifndef PARTICLE_H
#define PARTICLE_H

struct ParticleAttributes
{
	float3 position = { 0, 0, 0 };
	float3 velocity = { 0, 0, 0 };
	float3 acceleration = { 0, -9.82, 0 };
	float size = 0;
	bool sizeByLifetime = false;
	float rotation = 0;
	float rotationSpeed = 0;
	float lifetime = 0.0f;
	// Todo color
};

struct ByLifetimeParameter
{
	float4 start = {0, 0, 0, 0};
	float4 end = { 0, 0, 0, 0 };
	float4 changePerFrame = { 0, 0, 0, 0 };
};

class Particle
{
public:
	Particle();
	~Particle();

	void Update(double dt);

	bool IsAlive();

private:
	friend class ParticleEffect;

	ParticleAttributes m_Attributes;
	// ByLifetime interpolation values
	ByLifetimeParameter m_SizeByLifetime;

	void initValues(ParticleAttributes* startValues, ByLifetimeParameter* sizeByLifetime);
};

#endif
