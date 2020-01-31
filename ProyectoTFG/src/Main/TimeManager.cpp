#include "TimeManager.h"
#include <GLFW/glfw3.h>
#include "Timer.h"

double TimeManager::currentDeltaTime_;
double TimeManager::currentTime_;
std::list<Timer*> TimeManager::timers_;

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::CalculateDeltaTime()
{
	float t = glfwGetTime(); // time in seconds
	currentDeltaTime_ = t - currentTime_;
	currentTime_ = t;
}

void TimeManager::Init()
{
	currentDeltaTime_ = currentTime_ = 0;
}

void TimeManager::Release()
{
	for (Timer* timer : timers_) {
		delete timer;
		timer = nullptr;
	}
}

void TimeManager::Update()
{
	CalculateDeltaTime();
	for (Timer* timer : timers_) {
		timer->update(GetDeltaTime());
	}
}

double TimeManager::GetTimeSinceBeginning()
{
	return currentTime_;
}

double TimeManager::GetDeltaTime()
{
	return currentDeltaTime_;
}

Timer * TimeManager::createTimer()
{
	Timer* timer = new Timer();
	timers_.push_back(timer);

	return timer;
}
