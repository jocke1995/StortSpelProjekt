#ifndef OPTION_H
#define OPTION_H

#include <vector>
#include <fstream>
#include <utility>
#include "../Headers/Core.h"

//This is a singleton class in order to be globaly accessed
class Option {
public:
	static Option& GetInstance();

	//Read all variables from option file
	void ReadFile();
	//Write all variables to option file
	void WriteFile();

	float GetVariable(std::string name) const;
	void SetVariable(std::string name, const float value);

private:
	std::vector<std::pair<std::string, float>> m_Variables;
	
	Option() {};
};

#endif // !OPTION_H
