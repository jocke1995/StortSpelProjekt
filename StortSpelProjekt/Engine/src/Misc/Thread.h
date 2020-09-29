#ifndef THREAD_H
#define THREAD_H

#include <queue>
#include <mutex>
#include "Core.h"

class MultiThreadedTask;

class Thread
{
public:
	Thread();
	~Thread();

	bool IsTaskNullptr();

	void AddTask(MultiThreadedTask* task);
	void ExitThread();

	bool IsQueueEmpty();
private:
	HANDLE m_Thread;
	HANDLE m_BeginEvent;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<MultiThreadedTask*> m_TaskQueue;
	std::mutex m_Mutex;

	MultiThreadedTask* m_pTask= nullptr;

	bool m_IsRunning = true;
};

#endif