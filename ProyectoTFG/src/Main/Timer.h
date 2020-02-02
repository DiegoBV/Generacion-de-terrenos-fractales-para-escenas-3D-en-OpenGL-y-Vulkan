#pragma once
class Timer
{
private:
	double timeCount_;
	double timeLimit_;

public:
	Timer();
	~Timer();

	/// increments timeCount value
	void update(double deltaTime);
	/// resets timeCount
	void reset();

	/// sets the timer's limit in seconds to value
	void setTimeLimit(double value);
	/// returns true if timeCount is greater or equal than timeLimit
	bool hasFinished();
};

