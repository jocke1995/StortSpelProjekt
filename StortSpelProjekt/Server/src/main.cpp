#include "Engine.h"
#include "Misc/Thread.h"
#include "ConsoleCommand.h"

#include <iostream>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Network network;
	Engine engine = Engine();

	Console console;

	std::string str = "";

	engine.GetThreadPool()->AddTask(&console, FLAG_THREAD::NETWORK);

	while (strcmp(str.c_str(), "quit") != 0) 
	{
		console.GetInput(&str);

		if (strcmp(str.c_str(), "") != 0) {
			std::cout << str << std::endl;
			str = "";
			engine.GetThreadPool()->AddTask(&console, FLAG_THREAD::NETWORK);
		}
		std::cout << "Listen" << std::endl;
	}

	engine.GetThreadPool()->ExitThreads();

	return 0;
}