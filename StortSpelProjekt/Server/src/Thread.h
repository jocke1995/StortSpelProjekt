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
	NETWORK = BIT(3),
	// CopyTextures,
	// PrepareNextScene ..
	// etc
	ALL = BIT(4)
	// etc..
};

class Thread
{
public:
	Thread();
	~Thread();

	bool isLastActiveTaskNullptr();

	void AddTask(MultiThreadedTask* task, unsigned int taskFlag);
	void ExitThread();

	bool IsQueueEmpty();
	unsigned int GetTaskFlag();
private:
	HANDLE m_ThreadHandle;
	HANDLE m_EventHandle;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<MultiThreadedTask*> m_TaskQueue;
	std::mutex m_Mutex;

	MultiThreadedTask* m_pActiveTask= nullptr;
	unsigned int m_TaskFlag = 0;

	bool m_IsRunning = true;
};

#endif