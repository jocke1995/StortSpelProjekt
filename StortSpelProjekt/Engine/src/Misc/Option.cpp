#include "stdafx.h"
#include "Option.h"

Option& Option::GetInstance()
{
	static Option instance;

	return instance;
}

void Option::ReadFile()
{

}

void Option::WriteFile()
{
}

float Option::GetVariable(std::string name)
{
	//Search all variable names for one matching the given name
	for (unsigned int i = 0; i < m_Variables.size(); i++) {
		if (std::strcmp(m_Variables.at(i).first.c_str(), name.c_str()) == 0) {
			return m_Variables.at(i).second;
		}
	}

	//No variable with given name found log error and return NULL
	Log::PrintSeverity(Log::Severity::WARNING, "Attempt to access unknown option variable name: " + name + "\n");
	return NULL;
}

void Option::SetVariable(std::string name, const float value)
{
	//Search all variable names for one matching the given name
	for (unsigned int i = 0; i < m_Variables.size(); i++) {
		if (std::strcmp(m_Variables.at(i).first.c_str(), name.c_str())) {
			m_Variables.at(i).second = value;
		}
	}

	//No variable with given name found log error and create a new variable
	Log::PrintSeverity(Log::Severity::WARNING, "Attempting to set value to unknown option variable name: " + name + "; Creating a new variable!" + "\n");
	m_Variables.push_back(std::pair(name, value));
}
