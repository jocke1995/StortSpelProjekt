#include "TestHeaderTwo.h"

TestHeaderTwo::TestHeaderTwo()
{
	EventBus::GetInstance().Subscribe(this, &TestHeaderTwo::popMsg);
}

void TestHeaderTwo::popMsg(MessageLog* evnt)
{
	Log::Print("Printing from TestFileTwo, %s \n", evnt->msg.c_str());
}