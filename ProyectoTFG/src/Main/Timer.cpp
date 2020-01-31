#include "Timer.h"



Timer::Timer(): timeCount_(0), timeLimit_(0)
{
}

Timer::~Timer()
{
}

void Timer::update(double deltaTime)
{
	timeCount_ += deltaTime;
}

void Timer::reset()
{
	timeCount_ = 0;
}

void Timer::setTimeLimit(double value)
{
	timeLimit_ = value;
}

bool Timer::hasFinished()
{
	return timeCount_ >= timeLimit_;
}
