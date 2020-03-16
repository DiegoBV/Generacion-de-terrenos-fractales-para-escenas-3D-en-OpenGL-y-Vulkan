#pragma once
#include <list>

class Timer;
class TimeManager
{
private:
	static TimeManager* instance;
	std::list<Timer*> timers_;
	double currentTime_;
	double currentDeltaTime_;

	TimeManager();
	~TimeManager();

	/// Calculates delta time in seconds
	void calculateDeltaTime();

public:
	static TimeManager* GetSingleton();
	static void ShutDownSingleton();

	virtual void init();
	/// updates timers and calculates deltaTime and currentTime. Must be called before anything else.
	virtual void update();
	virtual void release();

	/// returns time since glfwInit()
	double getTimeSinceBeginning();
	/// returns delta time in seconds
	double getDeltaTime();
	/// creates a timer and stores it. Returns a pointer to it
	Timer* createTimer();
};

