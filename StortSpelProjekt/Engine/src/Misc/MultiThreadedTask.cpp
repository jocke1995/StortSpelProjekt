#include "stdafx.h"
#include "MultiThreadedTask.h"

MultiThreadedTask::MultiThreadedTask(unsigned int FLAG_THREAD)
{
	m_Flags = FLAG_THREAD;
	m_Flags |= FLAG_THREAD::ALL;
}

MultiThreadedTask::~MultiThreadedTask()
{
}
