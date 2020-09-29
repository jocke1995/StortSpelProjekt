#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>

class Thread;
class MultiThreadedTask;

class ThreadPool
{
public:
	ThreadPool(unsigned int nrOfThreads);
	~ThreadPool();

	void WaitForThreads(unsigned int flag);

	void AddTask(MultiThreadedTask* task);

	void ExitThreads();
private:
	std::vector<Thread*> m_Threads;

	unsigned int m_NrOfThreads;
	unsigned int m_ThreadCounter = 0;

	bool isAllLastActiveTasksFinished(unsigned int flag);
	bool isThreadsQueuesEmpty(unsigned int flag);
};

#endif
