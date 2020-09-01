#ifndef THREAD_H
#define THREAD_H

#include <queue>
#include <mutex>
#include "MultiThreadedTask.h"
#include "Core.h"


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
	HANDLE thread;
	HANDLE beginEvent;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<MultiThreadedTask*> taskQueue;
	std::mutex mutex;

	MultiThreadedTask* task= nullptr;
	unsigned int taskFlag = 0;

	bool isRunning = true;
};

#endif