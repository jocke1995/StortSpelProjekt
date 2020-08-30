#include "stdafx.h"
#include "Timer.h"

Timer::Timer(Window* window)
{
	this->window = window;

	auto start = std::chrono::system_clock::now();
	this->timeNow = start;
	this->timeLast = start;
}

Timer::~Timer()
{
}

void Timer::Update()
{
    static unsigned int fpsCounter = 0;
    fpsCounter++;

    // Calculate deltatime
    this->timeLast = this->timeNow;
    this->timeNow = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = this->timeNow - this->timeLast;
    this->dt = elapsed_time.count();

    // Set limit to the updates on the window title
    std::chrono::duration<double> elapsed_timeFps = this->timeNow - this->timeLastTitleUpdate;
    if (elapsed_timeFps.count() >= 1.0)
    {
        std::wstring fpsString = std::to_wstring(fpsCounter);
        window->SetWindowTitle(fpsString);
        this->timeLastTitleUpdate = this->timeNow;

        fpsCounter = 0;
    }
}

double Timer::GetDeltaTime() const
{
    return this->dt;
}

void Timer::StartTimer()
{
    this->start = std::chrono::system_clock::now();
}

double Timer::StopTimer()
{
    this->stop = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsedTime = this->stop - this->start;

    return elapsedTime.count();
}
