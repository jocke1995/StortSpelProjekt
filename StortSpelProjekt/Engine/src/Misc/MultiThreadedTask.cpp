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

void MultiThreadedTask::Activate()
{
	if (m_IsRunning == true)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "TASK ALREADY ACTIVE!\n");
	}
	m_IsRunning = true;
}

void MultiThreadedTask::Deactivate()
{
	if (m_IsRunning == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "TASK ALREADY DEACTIVATED!\n");
	}
	m_IsRunning = false;
}

const unsigned int MultiThreadedTask::GetThreadFlags() const
{
	return m_Flags;
}

const bool MultiThreadedTask::IsRunning() const
{
	return m_IsRunning;
}
