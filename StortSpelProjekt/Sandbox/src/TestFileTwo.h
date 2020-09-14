#pragma once
#include "Engine.h"

class TestFile
{
public:
	TestFile();
	~TestFile();
	void unsub();
private:
	void CalledFunction(TestEvent* evnt);
};