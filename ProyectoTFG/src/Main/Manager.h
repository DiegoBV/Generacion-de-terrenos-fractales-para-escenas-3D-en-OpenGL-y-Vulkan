#pragma once
class Manager
{
public:
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void release() = 0;
};

