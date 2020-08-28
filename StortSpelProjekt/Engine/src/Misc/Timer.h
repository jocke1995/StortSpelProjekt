#ifndef TIMER_H
#define TIMER_H

#include "Window.h"
#include <chrono>
class Timer
{
public:
	Timer(Window* window);
	~Timer();

	void Update();
	double GetDeltaTime() const;

	// For general cpu-time measurements
	void StartTimer();
	double StopTimer();
private:

	// For delta time and fpsTitle-update
	Window* window;
	double dt = 0.0f;
	std::chrono::time_point<std::chrono::system_clock> timeNow;
	std::chrono::time_point<std::chrono::system_clock> timeLast;
	std::chrono::time_point<std::chrono::system_clock> timeLastTitleUpdate;

	// For general time measurements
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> stop;
};

#endif