#include "stdafx.h"
#include "EngineRand.h"

EngineRand::EngineRand(unsigned long seed)
{
	m_Seed = seed;
	// values suggested by http://thompsonsed.co.uk/random-number-generators-for-c-performance-tested 
	m_Multiplier = 1664525;
	m_Incrementer = 1013904223;
	// 2^32 to big! therefore 2^31 was used.
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
	return (min + Rand() % (max - min));
}

float EngineRand::Randf()
{
	float nr = static_cast<float>(Rand()) / 10000;
	float deci = randDecimals();
	return nr + deci;
}

float EngineRand::Randf(unsigned long min, unsigned long max)
{
	return static_cast<float>(Rand(min, max)) + randDecimals();
}

float EngineRand::randDecimals()
{
	return static_cast<float>(Rand()) / m_Prime;
}
