#ifndef MULTITHREADEDTASK_H
#define MULTITHREADEDTASK_H

class MultiThreadedTask
{
public:
	MultiThreadedTask();
	~MultiThreadedTask();

	virtual void Execute() = 0;
};

#endif
