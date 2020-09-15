#pragma once
#include "Engine.h"

class TestFile
{
public:
	TestFile();
	~TestFile();
	void unsub();
	void sub();
private:
	void CalledFunction(TestEvent* evnt);
};