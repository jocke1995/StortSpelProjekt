#pragma once
#include "Engine.h"

class TestSubscriber {
public:
	TestSubscriber();
private:
	void popMsg(MessageLog* evnt);
};