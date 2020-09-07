#ifndef OPTION_H
#define OPTION_H

#include <string.h>
#include <vector>
#include <fstream>

//This is a singleton class in order to be globaly accessed
class Option {
public:
	static Option* getInstance();

	//Read all variables from option file
	void readFile();
	//Write all variables to option file
	void writeFile();

	float getVariable(std::string name);
	void setVariable(std::string name);

private:
	static Option* m_Instance;

	std::vector<std::string, float> m_Variables;

	Option() {};
};

#endif // !OPTION_H
