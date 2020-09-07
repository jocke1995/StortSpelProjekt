#include "stdafx.h"
#include "Option.h"

Option* Option::getInstance()
{
	if (!m_Instance) {
		m_Instance = new Option;
		return m_Instance;
	}
}

void Option::readFile()
{

}

void Option::writeFile()
{
}

float Option::getVariable(std::string name)
{
	for (unsigned int i = 0; i < m_Variables.size(); i++) {

	}

	return 0.0f;
}

void Option::setVariable(std::string name)
{
}
