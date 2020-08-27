#ifndef TASK_H
#define TASK_H

class Task
{
public:
	Task();
	~Task();

	virtual void Execute() = 0;
};

#endif
