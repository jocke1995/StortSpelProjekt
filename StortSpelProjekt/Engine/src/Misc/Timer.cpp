#include "stdafx.h"
#include "Timer.h"
#include "Window.h"

Timer::Timer()
{
    m_pWindow = nullptr;

    auto start = std::chrono::system_clock::now();
    m_TimeNow = start;
    m_TimeLast = start;
}

Timer::Timer(Window* window)
{
	m_pWindow = window;

	auto start = std::chrono::system_clock::now();
	m_TimeNow = start;
	m_TimeLast = start;
}

Timer::~Timer()
{
}

void Timer::Update()
{
    static unsigned int fpsCounter = 0;
    fpsCounter++;

    // Calculate deltatime
    m_TimeLast = m_TimeNow;
    m_TimeNow = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = m_TimeNow - m_TimeLast;
    m_Dt = elapsed_time.count();

    // Set limit to the updates on the window title
    std::chrono::duration<double> elapsed_timeFps = m_TimeNow - m_TimeLastTitleUpdate;
    // TODO: Make sure this solution works for all cases
    if (elapsed_timeFps.count() >= 1.0 && m_pWindow != nullptr)
    {
        std::wstring fpsString = std::to_wstring(fpsCounter);
        m_pWindow->SetWindowTitle(fpsString);
        m_TimeLastTitleUpdate = m_TimeNow;

        fpsCounter = 0;
    }
}

double Timer::GetDeltaTime() const
{
    return m_Dt;
}

void Timer::StartTimer()
{
    m_Start = std::chrono::system_clock::now();
}

double Timer::StopTimer()
{
    m_Stop = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsedTime = m_Stop - m_Start;
    return elapsedTime.count();
}
