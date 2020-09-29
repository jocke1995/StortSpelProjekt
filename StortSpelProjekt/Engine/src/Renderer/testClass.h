#ifndef TESTCLASS_H
#define TESTCLASS_H

#include "../Misc/MultiThreadedTask.h"

static unsigned int counter1 = 0;
static unsigned int counter2 = 0;
static unsigned int counter3 = 0;
class testClass0 : public MultiThreadedTask
{
public:
	testClass0()
	:MultiThreadedTask(FLAG_THREAD::TEST)
	{ };
	virtual ~testClass0() {};

	void Execute()
	{
		//Log::Print("Inside testClass0\n");
		counter1++;
	}

private:
};


class testClass1 : public MultiThreadedTask
{
public:
	testClass1()
		:MultiThreadedTask(FLAG_THREAD::TEST)
	{ };
	virtual ~testClass1() {};

	void Execute()
	{
		//Log::Print("Inside testClass1\n");
		counter2++;
	}

private:
};


class testClass2 : public MultiThreadedTask
{
public:
	testClass2()
		:MultiThreadedTask(FLAG_THREAD::TEST)
	{ };
	virtual ~testClass2() {};

	void Execute()
	{
		//Log::Print("Inside testClass2\n");
		counter3++;
		//while (true);
	}

private:
};

#endif