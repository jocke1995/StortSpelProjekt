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
	unsigned long Randu(unsigned long min, unsigned long max);
	unsigned long Rand(long min, long max);

	float Randf();
	float Randfu(unsigned long min, unsigned long max);
	float Randf(long min, long max);
private:

	float randDecimals();

	unsigned long m_Seed;
	unsigned long m_Multiplier;
	unsigned long m_Incrementer;
	unsigned long m_Prime;
};


#endif