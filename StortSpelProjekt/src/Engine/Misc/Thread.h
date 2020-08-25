#ifndef THREAD_H
#define THREAD_H

#include "Task.h"

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

	void AddTask(Task* task, unsigned int taskFlag);
	void ExitThread();

	bool IsQueueEmpty();
	unsigned int GetTaskFlag();
private:
	HANDLE thread;
	HANDLE beginEvent;

	static unsigned int __stdcall threadFunc(LPVOID lpParameter);

	std::queue<Task*> taskQueue;
	std::mutex mutex;

	Task* task= nullptr;
	unsigned int taskFlag = 0;

	bool isRunning = true;
};

#endif