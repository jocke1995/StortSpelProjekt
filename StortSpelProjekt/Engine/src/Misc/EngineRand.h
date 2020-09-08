#ifndef ENGINERAND_H
#define ENGINERAND_H

// This class generates pseudo random numbers
// via the Lehmer method https://en.wikipedia.org/wiki/Lehmer_random_number_generator
// Example of usage: https://www.youtube.com/watch?v=ZZY9YE7rZJw

class EngineRand
{
public:
	EngineRand(unsigned long seed = 0);
	void SetSeed(unsigned long seed);
	unsigned long Rand();
	unsigned long Rand(unsigned long min, unsigned long max);
private:
	unsigned long m_Seed;
	unsigned long m_Multiplier = 1664525;
	unsigned long m_Incrementer = 1013904223;
	unsigned long m_Prime;
};


#endif