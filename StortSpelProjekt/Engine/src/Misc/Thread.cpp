#include "stdafx.h"
#include "Thread.h"

unsigned int __stdcall Thread::threadFunc(LPVOID lpParameter)
{
	Thread* threadInstance = (Thread*)lpParameter;

	while (threadInstance->isRunning)
	{
		DWORD eventResult = WaitForSingleObject(
			threadInstance->beginEvent, // event handle
			INFINITE);    // indefinite wait

		// ------------------- Critical region 1-------------------
		threadInstance->mutex.lock();

		if (!threadInstance->taskQueue.empty())
		{
			// Get a task from the queue
			threadInstance->task = threadInstance->taskQueue.front();
			// Remove the task from the queue
			threadInstance->taskQueue.pop();
		}

		MultiThreadedTask* task = threadInstance->task;
		threadInstance->mutex.unlock();
		// ------------------- Critical region 1-------------------

		// Safetycheck if the thread has a task assigned
		if (task != nullptr)
		{
			task->Execute();

			// ------------------- Critical region 2-------------------
			threadInstance->mutex.lock();
			threadInstance->task = nullptr;
			threadInstance->mutex.unlock();
			// ------------------- Critical region 2-------------------
		}
	}
	return 0;
}

Thread::Thread()
{
	this->thread = (HANDLE)_beginthreadex(0, 0, this->threadFunc, this, 0, 0);
	SetThreadPriority(this->thread, THREAD_PRIORITY_TIME_CRITICAL);
	this->beginEvent = CreateEvent(
		NULL,               // default security attributes
		FALSE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL // object name
	);
}

Thread::~Thread()
{
	CloseHandle(this->thread);
}

bool Thread::IsTaskNullptr()
{
	this->mutex.lock();
	bool result = (this->task == nullptr);
	this->mutex.unlock();

	return result;
}

void Thread::ExitThread()
{
	this->isRunning = false;
}

void Thread::AddTask(MultiThreadedTask* task, unsigned int taskFlag)
{
	// Specify the type of task
	this->taskFlag = taskFlag;
	this->taskFlag |= FLAG_THREAD::ALL;
	
	// Add the task to the thread and start executing
	this->mutex.lock();
	this->taskQueue.push(task);
	if (!SetEvent(this->beginEvent))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetEvent in thread\n");
	}
	this->mutex.unlock();
}

bool Thread::IsQueueEmpty()
{
	return this->taskQueue.empty();
}

unsigned int Thread::GetTaskFlag()
{
	return this->taskFlag;
}
