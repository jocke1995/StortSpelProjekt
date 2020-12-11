#ifndef PARTICLE_H
#define PARTICLE_H

struct ParticleAttributes
{
	float3 position = { 0, 0, 0 };
	float3 velocity = { 0, 0, 0 };
	float3 acceleration = { 0, 0, 0 };
	float4 color = { 1, 1, 1, 1 }; // Texture gets multiplied by this float4
	float size = 1; // Total width/height of a particle
	float rotation = 0; // Start rotation of a particle
	float rotationSpeed = 0; // Update adds this to rotation each frame // TODO, make this to rotation by lifetime
	float lifetime = 0.0f; // How many seconds left a particle has to live
};

class Particle
{
public:
	Particle();
	~Particle();

	void Update(double dt, float sizeByLifetime, float4 colorByLifetime);

	bool IsAlive();

private:
	friend class ParticleEffect;

	ParticleAttributes m_Attributes;

	void initValues(ParticleAttributes* startValues);
};

#endif
