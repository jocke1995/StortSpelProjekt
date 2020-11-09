#include "stdafx.h"
#include "Particle.h"

Particle::Particle()
{
	initDefaultValues();
}

Particle::~Particle()
{
}

void Particle::Update(double dt)
{
	m_Position = m_Position + m_Velocity * dt;

	changeVelocity(dt);
	m_Lifetime -= dt;
}

bool Particle::IsAlive()
{
	return m_Lifetime > 0;
}

void Particle::initDefaultValues()
{
	m_Position = { 0, 0, 0 };
	m_Velocity = { 0, 5, 0 };
	m_Gravity = 1;
	m_Size = 1;
	m_Rotation = 0;
	m_Lifetime = 0.0f;
}

void Particle::changeVelocity(float dt)
{
	m_Velocity.y -= m_Gravity;
}
