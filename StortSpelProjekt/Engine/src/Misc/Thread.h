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
	const unsigned int GetActiveTaskThreadFlag() const;

	void AddTask(MultiThreadedTask* task);
	void ExitThread();

	bool IsQueueEmpty();
private:
	HANDLE m_Thread;
	HANDLE m_Event;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<MultiThreadedTask*> m_TaskQueue;
	std::mutex m_Mutex;

	MultiThreadedTask* m_pActiveTask = nullptr;

	bool m_IsRunning = true;
};
#endif
