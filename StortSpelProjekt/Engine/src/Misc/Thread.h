#ifndef THREAD_H
#define THREAD_H

#include <queue>
#include <mutex>
#include "Core.h"

class MultiThreadedTask;

enum FLAG_THREAD
{
	RENDER = BIT(1),
	COPY_DATA = BIT(2),
	// CopyTextures,
	// PrepareNextScene ..
	// etc
	ALL = BIT(3)
	// etc..
};

class Thread
{
public:
	Thread();
	~Thread();

	bool IsTaskNullptr();

	void AddTask(MultiThreadedTask* task, unsigned int taskFlag);
	void ExitThread();

	bool IsQueueEmpty();
	unsigned int GetTaskFlag();
private:
	HANDLE m_Thread;
	HANDLE m_BeginEvent;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<MultiThreadedTask*> m_TaskQueue;
	std::mutex m_Mutex;

	MultiThreadedTask* m_pTask= nullptr;
	unsigned int m_TaskFlag = 0;

	bool m_IsRunning = true;
};

#endif