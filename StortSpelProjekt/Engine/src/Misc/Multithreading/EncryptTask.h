#ifndef CALCULATEHEIGHTMAPNORMALSTASK_H
#define CALCULATEHEIGHTMAPNORMALSTASK_H

#include "MultiThreadedTask.h"

class EncryptTask : public MultiThreadedTask
{
public:
	EncryptTask(unsigned int key, const std::string path);
	void Execute();
private:
	unsigned int m_Key;
	const std::string m_Path;
};

#endif