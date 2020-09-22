#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H
#include "Engine.h"
#include "Misc/MultiThreadedTask.h"

class Console : public MultiThreadedTask
{
public:
	Console();

	void Execute();

	void GetInput(std::string* str);

private:
	std::string m_ConsoleInput;
	bool m_Done;

};


#endif // !CONSOLE_COMMAND_H
