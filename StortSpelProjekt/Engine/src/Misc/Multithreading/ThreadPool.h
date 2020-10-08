#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>

class Thread;
class MultiThreadedTask;

class ThreadPool
{
public:
	~ThreadPool();

	void WaitForThreads(unsigned int flag);

	void AddTask(MultiThreadedTask* task);

	static ThreadPool& GetInstance(unsigned int nrOfThreads = 0);
	unsigned int GetNrOfThreads() const;
private:
	ThreadPool(unsigned int nrOfThreads);
	std::vector<Thread*> m_Threads;

	unsigned int m_NrOfThreads;
	unsigned int m_ThreadCounter = 0;

	bool isAllLastActiveTasksFinished(unsigned int flag);
	bool isThreadsQueuesEmpty(unsigned int flag);
	void exitThreads();
};

#endif
