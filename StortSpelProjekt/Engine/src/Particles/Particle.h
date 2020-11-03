#ifndef PARTICLE_H
#define PARTICLE_H

class Particle
{
public:
	Particle();
	~Particle();

	void Update(double dt);

private:
	float3 m_Position;
	float3 m_Velocity;
	float m_Size;
	float m_Rotation; // Only rotates from camera perspective
	float m_Lifetime;
	// Todo color

	void initDefaultValues();
};

#endif
