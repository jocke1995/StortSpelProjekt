#ifndef PARTICLE_H
#define PARTICLE_H

struct ParticleStartValues
{
	float3 position = { 0, 0, 0 };
	float3 velocity = { 0, 18, 0 };
	float3 acceleration = { 0, 9.82, 0 };
	float size = 1;
	float rotation = 0;
	float lifetime = 3.0f;
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

	float3 m_Position;
	float3 m_Velocity;
	float3 m_Acceleration;
	float m_Size;
	float m_Rotation; // Only rotates from camera perspective
	float m_Lifetime;
	// Todo color

	void initDefaultValues(ParticleStartValues* defaultValues);

	void changeVelocity(float dt);
};

#endif
