#include "stdafx.h"
#include "Option.h"

Option& Option::GetInstance()
{
	static Option instance;

	return instance;
}

void Option::ReadFile()
{
	std::ifstream file;

	file.open("config.txt");
	if(!file.is_open()) 
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Attempted to open non-existing config file");
	}
	else
	{

		m_Variables.clear();
		int i = 0;
		while (!file.eof())
		{
			std::string tempName;
			std::string tempValue;

			std::getline(file, tempName, ' ');
			std::getline(file, tempValue, '\n');

			if (tempName != "")
				m_Variables.push_back(std::pair(tempName, tempValue));
		}
	}
}

void Option::WriteFile()
{
	std::ofstream file;

	file.open("config.txt");
	if (!file.is_open()) 
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Attempted to open non-existing config file; Creating a new one");
	}

	for (int i = 0; i < m_Variables.size(); i++) 
	{
		file << m_Variables.at(i).first;
		file << " ";
		file << m_Variables.at(i).second;
		file << "\n";
	}
}

std::string Option::GetVariable(std::string name) const
{
	//Search all variable names for one matching the given name
	for (unsigned int i = 0; i < m_Variables.size(); i++) 
	{
		if (std::strcmp(m_Variables.at(i).first.c_str(), name.c_str()) == 0) 
		{
			return m_Variables.at(i).second;
		}
	}

	//No variable with given name found log error and return NULL
	Log::PrintSeverity(Log::Severity::WARNING, "Attempt to access unknown option variable name: " + name + "\n");
	return NULL;
}

void Option::SetVariable(std::string name, const std::string value)
{
	//Search all variable names for one matching the given name
	bool found = false;
	for (unsigned int i = 0; i < m_Variables.size(); i++) 
	{
		if (std::strcmp(m_Variables.at(i).first.c_str(), name.c_str()) == 0) 
		{
			m_Variables.at(i).second = value;
			found = true;
			break;
		}
	}

	//No variable with given name found log error and create a new variable
	if (!found)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Attempting to set value to unknown option variable name: " + name + "; Creating a new variable!" + "\n");
		m_Variables.push_back(std::pair(name, value));
	}
}
