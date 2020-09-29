#include "stdafx.h"
#include "ThreadPool.h"
#include "Thread.h"
ThreadPool::ThreadPool(int nrOfThreads)
{
	m_NrOfThreads = nrOfThreads;
	
	// Create Threads
	for (int i = 0; i < m_NrOfThreads; i++)
		m_Threads.push_back(new Thread());
}

ThreadPool::~ThreadPool()
{
	for (Thread* thread : m_Threads)
		delete thread;
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

		if (isEmpty && isAllFinished(flag))
		{
			break;
		}
	}
}

void ThreadPool::AddTask(MultiThreadedTask* task, unsigned int flag)
{
	// Adds a m_pTask to a m_Thread
	m_Threads.at(m_ThreadCounter % m_NrOfThreads)->AddTask(task, flag);
	m_ThreadCounter++;
}

void ThreadPool::ExitThreads()
{
	for (auto thread : m_Threads)
	{
		thread->ExitThread();
	}
}

bool ThreadPool::isAllFinished(unsigned int flag)
{
	for (Thread* thread : m_Threads)
	{
		if (thread->GetTaskFlag() & flag)
		{
			if (thread->isTaskNullptr() == false)
			{
				return false;
			}
		}
	}
	return true;
}

bool ThreadPool::isThreadsQueuesEmpty(unsigned int flag)
{
	for (auto thread : m_Threads)
	{
		if (thread->GetTaskFlag() & flag)
		{
			if (thread->IsQueueEmpty() == false)
			{
				return false;
			}
		}
	}
	return true;
}
