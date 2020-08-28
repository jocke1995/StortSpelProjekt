#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Thread.h"
class ThreadPool
{
public:
	ThreadPool(int nrOfThreads);
	~ThreadPool();

	void WaitForThreads(unsigned int flag);

	void AddTask(Task* task, unsigned int flag);

	void ExitThreads();
private:
	std::vector<Thread*> threads;

	int nrOfThreads;
	unsigned int threadCounter = 0;

	bool IsAllFinished(unsigned int flag);
	bool IsThreadsQueuesEmpty(unsigned int flag);
};

#endif
