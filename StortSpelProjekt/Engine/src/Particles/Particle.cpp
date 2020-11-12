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

	m_Attributes.position = m_Attributes.position + m_Attributes.velocity * dt;

	m_Attributes.rotation = m_Attributes.rotation + m_Attributes.rotationSpeed * dt;
	
	m_Attributes.lifetime -= dt;
}

bool Particle::IsAlive()
{
	return m_Attributes.lifetime > 0;
}

void Particle::initDefaultValues(ParticleAttributes* defaultValues)
{
	m_Attributes = *defaultValues;
}

void Particle::changeVelocity(float dt)
{
	m_Attributes.velocity = m_Attributes.velocity - m_Attributes.acceleration *dt;
}
