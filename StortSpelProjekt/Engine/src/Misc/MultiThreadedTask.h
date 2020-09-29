#ifndef MULTITHREADEDTASK_H
#define MULTITHREADEDTASK_H

#include "Core.h"

class MultiThreadedTask
{
public:
	MultiThreadedTask(unsigned int FLAG_THREAD);
	~MultiThreadedTask();

	virtual void Execute() = 0;

	const unsigned int GetThreadFlags() const;

private:
	unsigned int m_Flags = 0;
};

#endif
