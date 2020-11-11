#include "stdafx.h"
#include "Particle.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Update(double dt)
{
	changeVelocity(dt);

	m_Position = m_Position + m_Velocity * dt;
	
	m_Lifetime -= dt;
}

bool Particle::IsAlive()
{
	return m_Lifetime > 0;
}

void Particle::initDefaultValues(ParticleStartValues* defaultValues)
{
	m_Position = defaultValues->position;
	m_Velocity = defaultValues->velocity;
	m_Acceleration = defaultValues->acceleration;
	m_Size = defaultValues->size;
	m_Rotation = defaultValues->rotation;
	m_Lifetime = defaultValues->lifetime;
}

void Particle::changeVelocity(float dt)
{
	m_Velocity = m_Velocity - m_Acceleration*dt;
}
