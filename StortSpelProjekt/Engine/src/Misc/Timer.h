#ifndef TIMER_H
#define TIMER_H

#include <chrono>
class Window;
class Timer
{
public:
	//Default constructor
	Timer();
	Timer(Window* window);
	~Timer();

	void Update();
	double GetDeltaTime() const;

	// For general cpu-time measurements
	void StartTimer();
	double StopTimer();
private:

	// For delta time and fpsTitle-update
	Window* m_pWindow;
	double m_Dt = 0.0f;
	std::chrono::time_point<std::chrono::system_clock> m_TimeNow;
	std::chrono::time_point<std::chrono::system_clock> m_TimeLast;
	std::chrono::time_point<std::chrono::system_clock> m_TimeLastTitleUpdate;

	// For general time measurements
	std::chrono::time_point<std::chrono::system_clock> m_Start;
	std::chrono::time_point<std::chrono::system_clock> m_Stop;
};

#endif