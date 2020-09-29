#ifndef THREAD_H
#define THREAD_H

#include <queue>
#include <mutex>
#include "Core.h"

class MultiThreadedTask;

class Thread
{
public:
	Thread(unsigned int threadId);
	~Thread();

	bool IsTaskNullptr();

	void AddTask(MultiThreadedTask* task);
	void ExitThread();

	bool IsQueueEmpty();
	bool WakeUpThread();
private:
	HANDLE m_ThreadHandle;
	HANDLE m_EventHandle;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<MultiThreadedTask*> m_TaskQueue;
	std::mutex m_Mutex;

	MultiThreadedTask* m_pActiveTask = nullptr;

	bool m_IsRunning = true;
	unsigned int m_ThreadId = 0;
};
#endif
