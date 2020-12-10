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
	m_Attributes.velocity = m_Attributes.velocity + m_Attributes.acceleration * dt;

	m_Attributes.position = m_Attributes.position + m_Attributes.velocity * dt;

	m_Attributes.rotation = m_Attributes.rotation + m_Attributes.rotationSpeed * dt;

	m_Attributes.lifetime -= dt;

	m_Attributes.size -= m_SizeByLifetime.changePerFrame.x * dt;
}

bool Particle::IsAlive()
{
	return m_Attributes.lifetime > 0;
}

void Particle::initValues(ParticleAttributes* startValues, ByLifetimeParameter* sizeByLifetime)
{
	m_Attributes = *startValues;
	m_SizeByLifetime = *sizeByLifetime;
	// Setup interpolation values for byLifetime
	m_SizeByLifetime.changePerFrame.x = (m_SizeByLifetime.start.x - m_SizeByLifetime.end.x) / m_Attributes.lifetime;
}
