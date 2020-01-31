#pragma once
#include <list>

class Timer;
class TimeManager
{
private:
	static std::list<Timer*> timers_;
	static double currentTime_;
	static double currentDeltaTime_;

	TimeManager();
	~TimeManager();

	// Calculates delta time in seconds
	static void CalculateDeltaTime();

public:
	static void Init();
	static void Release();
	// updates timers and calculates deltaTime and currentTime. Must be called before anything else.
	static void Update();

	// returns time since glfwInit()
	static double GetTimeSinceBeginning();
	// returns delta time in seconds
	static double GetDeltaTime();
	// creates a timer and stores it. Returns a pointer to it
	static Timer* createTimer();
};

