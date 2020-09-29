#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>

class Thread;
class MultiThreadedTask;
class ThreadPool
{
public:
	ThreadPool(int nrOfThreads);
	~ThreadPool();

	void WaitForThreads(unsigned int flag);

	void AddTask(MultiThreadedTask* task);

	void ExitThreads();
private:
	std::vector<Thread*> m_Threads;

	int m_NrOfThreads;
	unsigned int m_ThreadCounter = 0;

	bool isAllFinished(unsigned int flag);
	bool isThreadsQueuesEmpty(unsigned int flag);
};

#endif
