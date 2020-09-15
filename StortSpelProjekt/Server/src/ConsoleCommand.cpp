#include "ConsoleCommand.h"
#include <iostream>

Console::Console()
{
	m_ConsoleInput = "";
}

void Console::Execute()
{
	std::cin >> m_ConsoleInput;
}

void Console::GetInput(std::string* str)
{
	*str = m_ConsoleInput;
}
