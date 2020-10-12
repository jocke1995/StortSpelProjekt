#include "stdafx.h"
#include "ThreadPool.h"
#include "Thread.h"
ThreadPool::ThreadPool(unsigned int nrOfThreads)
{
	m_NrOfThreads = nrOfThreads;
	
	// Create Threads
	for (int i = 0; i < m_NrOfThreads; i++)
	{
		m_Threads.push_back(new Thread(i));
	}
}

ThreadPool::~ThreadPool()
{
	exitThreads();

	for (Thread* thread : m_Threads)
	{
		delete thread;
	}
}

void ThreadPool::WaitForThreads(unsigned int flag)
{
	// Two conditios to wait.
	// 1. Until each m_TaskQueue of each m_Thread is empty
	// 2. Until each m_Thread's taskpointer is nullptr
	// Otherwise there is a chance that the mainthread will continue
	// whilst some m_Threads are working on some of the last tasks.

	// Wait until all tasks are completed
	bool isEmpty = false;
	while (true)
	{
		isEmpty = isThreadsQueuesEmpty(flag);

		if (isEmpty && isAllLastActiveTasksFinished(flag))
		{
			break;
		}
	}
}

void ThreadPool::AddTask(MultiThreadedTask* task)
{
	// Adds a m_pTask to a m_Thread
	m_Threads.at(m_ThreadCounter % m_NrOfThreads)->addTask(task);
	m_ThreadCounter++;
}

ThreadPool& ThreadPool::GetInstance(unsigned int nrOfThreads)
{
	static ThreadPool instance(nrOfThreads);
	return instance;
}

unsigned int ThreadPool::GetNrOfThreads() const
{
	return m_NrOfThreads;
}

bool ThreadPool::isAllLastActiveTasksFinished(unsigned int flag)
{
	for (Thread* thread : m_Threads)
	{
		if (thread->isLastActiveTaskNullptr(flag) == false)
		{
			return false;
		}
	}
	return true;
}

bool ThreadPool::isThreadsQueuesEmpty(unsigned int flag)
{
	for (auto thread : m_Threads)
	{
		if (thread->isQueueEmptyFromTasksWithSpecifiedFlags(flag) == false)
		{
			return false;
		}
	}
	return true;
}


void ThreadPool::exitThreads()
{
	WaitForThreads(FLAG_THREAD::ALL);
	std::vector<HANDLE> handles;
	for (auto thread : m_Threads)
	{
		handles.push_back(thread->m_ThreadHandle);
		thread->exitThread();
	}
	WaitForMultipleObjects(m_NrOfThreads, handles.data(), true, INFINITE);
}
