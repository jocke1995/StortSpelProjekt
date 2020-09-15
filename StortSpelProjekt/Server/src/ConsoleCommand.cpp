#include "ConsoleCommand.h"
#include <iostream>

Console::Console()
{
	m_ConsoleInput = "";
	m_Done = false;
}

void Console::Execute()
{
	std::cin >> m_ConsoleInput;
	m_Done = true;
}

void Console::GetInput(std::string* str)
{
	if (m_Done)
	{
		*str = m_ConsoleInput;
		m_ConsoleInput = "";
		m_Done = false;
	}
}
