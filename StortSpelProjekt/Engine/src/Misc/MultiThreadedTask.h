#ifndef MULTITHREADEDTASK_H
#define MULTITHREADEDTASK_H

#include "Core.h"

class MultiThreadedTask
{
public:
	MultiThreadedTask(unsigned int FLAG_THREAD);
	~MultiThreadedTask();

	virtual void Execute() = 0;

	void Activate();
	void Deactivate();

	const unsigned int GetThreadFlags() const;
	const bool IsRunning() const;

private:
	unsigned int m_Flags = 0;
	bool m_IsRunning = false;
};

#endif
