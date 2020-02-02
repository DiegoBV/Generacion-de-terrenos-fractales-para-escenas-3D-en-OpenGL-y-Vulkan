#include "TimeManager.h"
#include <GLFW/glfw3.h>
#include "Timer.h"

TimeManager* TimeManager::instance = nullptr;

TimeManager* TimeManager::GetSingleton()
{
	if (instance == nullptr) instance = new TimeManager();
	return instance;
}

void TimeManager::ShutDownSingleton()
{
	if (instance != nullptr) {
		instance->release();
		delete instance; instance = nullptr;
	}
}

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::calculateDeltaTime()
{
	float t = glfwGetTime(); // time in seconds
	currentDeltaTime_ = t - currentTime_;
	currentTime_ = t;
}

void TimeManager::init()
{
	currentDeltaTime_ = currentTime_ = 0;
}

void TimeManager::release()
{
	for (Timer* timer : timers_) {
		delete timer;
		timer = nullptr;
	}
}

void TimeManager::update()
{
	calculateDeltaTime();
	for (Timer* timer : timers_) {
		timer->update(getDeltaTime());
	}
}

double TimeManager::getTimeSinceBeginning()
{
	return currentTime_;
}

double TimeManager::getDeltaTime()
{
	return currentDeltaTime_;
}

Timer * TimeManager::createTimer()
{
	Timer* timer = new Timer();
	timers_.push_back(timer);

	return timer;
}
