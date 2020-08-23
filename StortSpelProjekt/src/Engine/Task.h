#pragma once

class Task
{
public:
	Task();
	~Task();

	virtual void Execute() = 0;
};

