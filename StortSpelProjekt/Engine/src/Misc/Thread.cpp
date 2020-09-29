#include "stdafx.h"
#include "Thread.h"
#include "MultiThreadedTask.h"

unsigned int __stdcall Thread::threadFunc(LPVOID lpParameter)
{
	Thread* threadInstance = (Thread*)lpParameter;
	bool waitNextIteration = true;

	while (threadInstance->m_IsRunning)
	{
		if (waitNextIteration == true)
		{
			DWORD eventResult = WaitForSingleObject(
				threadInstance->m_Event, // event handle
				INFINITE);    // indefinite wait
		}
		
		// ------------------- Critical region 1-------------------
		if (threadInstance->m_TaskQueue.empty() == false)
		{
			threadInstance->m_Mutex.lock();

			// Get a task from the queue
			threadInstance->m_pActiveTask = threadInstance->m_TaskQueue.front();

			// Remove the m_pTask from the queue
			threadInstance->m_TaskQueue.pop();

			threadInstance->m_Mutex.unlock();
		}
		// ------------------- Critical region 1-------------------

		if (threadInstance->m_pActiveTask != nullptr)
		{
			threadInstance->m_pActiveTask->Execute();

			// ------------------- Critical region 2-------------------
			threadInstance->m_Mutex.lock();
			threadInstance->m_pActiveTask = nullptr;

			// If the main thread adds a new task while this thread is working, we won't wait next iteration.
			if (threadInstance->m_TaskQueue.empty() == true)
			{
				waitNextIteration = true;
			}
			else
			{
				waitNextIteration = false;
			}

			threadInstance->m_Mutex.unlock();
		}
		// ------------------- Critical region 2-------------------
	}
	return 0;
}

Thread::Thread()
{
	m_Thread = (HANDLE)_beginthreadex(0, 0, threadFunc, this, 0, 0);
	SetThreadPriority(m_Thread, THREAD_PRIORITY_TIME_CRITICAL);
	m_Event = CreateEvent(
		NULL,               // default security attributes
		FALSE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL // object m_Name
	);
}

Thread::~Thread()
{
	CloseHandle(m_Thread);
}

bool Thread::IsTaskNullptr()
{
	bool result = false;

	m_Mutex.lock();
	if (m_pActiveTask == nullptr)
	{
		result = true;
	}
	m_Mutex.unlock();

	return result;
}

const unsigned int Thread::GetActiveTaskThreadFlag() const
{
	return m_pActiveTask->GetThreadFlags();
}

void Thread::ExitThread()
{
	m_IsRunning = false;
}

void Thread::AddTask(MultiThreadedTask* task)
{
	// Add the m_pTask to the m_Thread and m_Start executing
	m_Mutex.lock();
	m_TaskQueue.push(task);

#ifdef _DEBUG
	// Start the thread and catch errors (if any)
	bool eventError = SetEvent(m_Event);

	if (eventError == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetEvent in thread\n");
	}
#else
	SetEvent(m_Event);
#endif
	
	m_Mutex.unlock();
}

bool Thread::IsQueueEmpty()
{
	bool isEmpty = false;
	m_Mutex.lock();
	isEmpty = m_TaskQueue.empty();
	m_Mutex.unlock();

	return isEmpty;
}
