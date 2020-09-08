#include "stdafx.h"
#include "EngineRand.h"

EngineRand::EngineRand(unsigned long seed)
{
	m_Seed = seed;
	m_Prime = static_cast<unsigned long>(std::pow(2, 31));
}

void EngineRand::SetSeed(unsigned long seed)
{
	m_Seed = seed;
}

unsigned long EngineRand::Rand()
{
	m_Seed = (m_Multiplier * m_Seed + m_Incrementer) % m_Prime;
	return m_Seed;
}

unsigned long EngineRand::Rand(unsigned long min, unsigned long max)
{
	return (min + Rand() % (max - min + 1));
}
