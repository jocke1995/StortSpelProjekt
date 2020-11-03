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
}

void Particle::initDefaultValues()
{
	m_Position = { 0, 0, 0 };
	m_Velocity = { 0, 0, 0 };
	m_Size = { 1 };
	m_Rotation = { 0 };
	m_Lifetime = { 0 };
}
