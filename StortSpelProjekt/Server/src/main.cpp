#include "Engine.h"
#include "ConsoleCommand.h"
#include "ThreadPool.h"
#include "Thread.h"
#include "MultiThreadedTask.h"

#include <iostream>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::vector<Network*> clients;

	int nrOfClients = 0;

	// ThreadPool
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0)
	{
		numCores = 1; // function not supported
	}
	ThreadPool* threadPool = new ThreadPool(numCores); // Set num m_Threads to number of cores of the cpu

	Console console;

	std::string str = "";
	threadPool->AddTask(&console, FLAG_THREAD::NETWORK);

	while (strcmp(str.c_str(), "quit") != 0) 
	{
		str = "";
		console.GetInput(&str);

		if (strcmp(str.c_str(), "") != 0)
		{
			std::cout << str << std::endl;
			threadPool->AddTask(&console, FLAG_THREAD::NETWORK);
		}
		if (strcmp(str.c_str(), "AddClient") == 0)
		{

		}
	}

	threadPool->ExitThreads();
	delete threadPool;

	return 0;
}