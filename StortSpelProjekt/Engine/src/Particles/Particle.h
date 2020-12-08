#ifndef PARTICLE_H
#define PARTICLE_H

struct ParticleAttributes
{
	float3 position = { 0, 0, 0 };
	float3 velocity = { 0, 0, 0 };
	float3 acceleration = { 0, -9.82, 0 };
	float size = 0;
	float rotation = 0;
	float rotationSpeed = 0;
	float lifetime = 0.0f;
	// Todo color
};

class Particle
{
public:
	Particle();
	~Particle();

	void RenderUpdate(double dt);

	bool IsAlive();

private:
	friend class ParticleEffect;

	ParticleAttributes m_Attributes;

	void initValues(ParticleAttributes* startValues);

	void changeVelocity(float dt);
};

#endif
