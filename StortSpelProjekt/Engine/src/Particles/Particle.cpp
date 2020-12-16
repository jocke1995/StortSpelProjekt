#include "stdafx.h"
#include "Particle.h"

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Update(double dt, float sizeChange, float4 colorChange)
{
	m_Attributes.velocity = m_Attributes.velocity + m_Attributes.acceleration * dt;
	m_Attributes.position = m_Attributes.position + m_Attributes.velocity * dt;
	m_Attributes.rotation = m_Attributes.rotation + m_Attributes.rotationSpeed * dt;
	m_Attributes.lifetime -= dt;

	m_Attributes.size -= sizeChange * dt;
	m_Attributes.color -= colorChange * dt;
}

bool Particle::IsAlive()
{
	return m_Attributes.lifetime > 0;
}

void Particle::initValues(ParticleAttributes* startValues)
{
	m_Attributes = *startValues;
}