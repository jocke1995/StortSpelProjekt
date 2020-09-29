#include "stdafx.h"
#include "Thread.h"
#include "MultiThreadedTask.h"

unsigned int __stdcall Thread::threadFunc(void* lpParameter)
{
	Thread* threadInstance = (Thread*)lpParameter;
	bool waitNextIteration = true;

	while (threadInstance->m_IsRunning)
	{
		if (waitNextIteration == true)
		{
			DWORD eventResult = WaitForSingleObject(
				threadInstance->m_EventHandle, // event handle
				INFINITE);    // indefinite wait
		}
		
		// ------------------- Critical region 1-------------------
		if (threadInstance->m_TaskDeque.empty() == false)
		{
			threadInstance->m_Mutex.lock();

			// Get a task from the deque
			threadInstance->m_pActiveTask = threadInstance->m_TaskDeque.front();

			// Remove the m_pTask from the deque
			threadInstance->m_TaskDeque.pop_front();

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
			if (threadInstance->m_TaskDeque.empty() == true)
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

#ifdef _DEBUG
	Log::Print("Engine thread with id:%d Exiting!\n", threadInstance->m_ThreadId);
#endif
	return 0;
}

Thread::Thread(unsigned int threadId)
{
	m_ThreadId = threadId;

	// Create and start the thread function
	m_ThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc, static_cast<void*>(this), 0, 0));

	// Set Thread Priority
	if (SetThreadPriority(m_ThreadHandle, THREAD_PRIORITY_TIME_CRITICAL) == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to 'SetThreadPriority' belonging to a thread with id: %d\n", m_ThreadId);
	}

	// Create an event for the thread
	m_EventHandle = CreateEvent(
		NULL,   // default security attributes
		FALSE,  // manual-reset event
		FALSE,  // initial state is nonsignaled
		NULL	// object m_Name
	);
}

Thread::~Thread()
{
	if (CloseHandle(m_ThreadHandle) == false)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Failed to 'CloseHandle' belonging to a thread with id: %d\n", m_ThreadId);
	}
}

bool Thread::isLastActiveTaskNullptr(unsigned int flag)
{
	bool result = false;

	m_Mutex.lock();
	if (m_pActiveTask == nullptr)
	{
		result = true;
	}
	else if(m_pActiveTask->GetThreadFlags() & flag == 0)
	{
		result = true;
	}
	m_Mutex.unlock();

	return result;
}

void Thread::exitThread()
{
	m_IsRunning = false;
	wakeUpThread();
}

void Thread::addTask(MultiThreadedTask* task)
{
	// Add the m_pTask to the m_Thread and m_Start executing
	m_Mutex.lock();
	m_TaskDeque.push_back(task);

#ifdef _DEBUG
	// Start the thread and catch errors (if any)
	bool eventError = wakeUpThread();

	if (eventError == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to wake up thread\n");
	}
#else
	wakeUpThread();
#endif
	
	m_Mutex.unlock();
}

bool Thread::isQueueEmptyFromTasksWithSpecifiedFlags(unsigned int flag)
{
	bool isEmptyFromTasksWithSpecifiedFlag = true;
	m_Mutex.lock();
	for (MultiThreadedTask* task : m_TaskDeque)
	{
		if (task->GetThreadFlags() & flag)
		{
			isEmptyFromTasksWithSpecifiedFlag = false;
			break;
		}
	}
	m_Mutex.unlock();

	return isEmptyFromTasksWithSpecifiedFlag;
}

bool Thread::wakeUpThread()
{
	return SetEvent(m_EventHandle);
}
