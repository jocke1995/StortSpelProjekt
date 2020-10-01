#include "stdafx.h"
#include "Thread.h"
#include "MultiThreadedTask.h"

unsigned int __stdcall Thread::threadFunc(LPVOID lpParameter)
{
	Thread* threadInstance = (Thread*)lpParameter;

	while (threadInstance->m_IsRunning)
	{
		DWORD eventResult = WaitForSingleObject(
			threadInstance->m_EventHandle, // event handle
			INFINITE);    // indefinite wait

		// ------------------- Critical region 1-------------------
		threadInstance->m_Mutex.lock();

		if (!threadInstance->m_TaskQueue.empty())
		{
			// Get a m_pTask from the queue
			threadInstance->m_pActiveTask = threadInstance->m_TaskQueue.front();
			// Remove the m_pTask from the queue
			threadInstance->m_TaskQueue.pop();
		}

		MultiThreadedTask* task = threadInstance->m_pActiveTask;
		threadInstance->m_Mutex.unlock();
		// ------------------- Critical region 1-------------------

		// Safetycheck if the m_Thread has a m_pTask assigned
		if (task != nullptr)
		{
			task->Execute();

			// ------------------- Critical region 2-------------------
			threadInstance->m_Mutex.lock();
			threadInstance->m_pActiveTask = nullptr;
			threadInstance->m_Mutex.unlock();
			// ------------------- Critical region 2-------------------
		}
	}
	return 0;
}

Thread::Thread()
{
	m_ThreadHandle = (HANDLE)_beginthreadex(0, 0, threadFunc, this, 0, 0);
	SetThreadPriority(m_ThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	m_EventHandle = CreateEvent(
		NULL,               // default security attributes
		FALSE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL // object m_Name
	);
}

Thread::~Thread()
{
	CloseHandle(m_ThreadHandle);
}

bool Thread::isLastActiveTaskNullptr()
{
	m_Mutex.lock();
	bool result = (m_pActiveTask == nullptr);
	m_Mutex.unlock();

	return result;
}

void Thread::ExitThread()
{
	m_IsRunning = false;
}

void Thread::AddTask(MultiThreadedTask* task, unsigned int taskFlag)
{
	// Specify the type of m_pTask
	m_TaskFlag = taskFlag;
	m_TaskFlag |= FLAG_THREAD::ALL;
	
	// Add the m_pTask to the m_Thread and m_Start executing
	m_Mutex.lock();
	m_TaskQueue.push(task);
	if (!SetEvent(m_EventHandle))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetEvent in thread\n");
	}
	m_Mutex.unlock();
}

bool Thread::IsQueueEmpty()
{
	return m_TaskQueue.empty();
}

unsigned int Thread::GetTaskFlag()
{
	return m_TaskFlag;
}
