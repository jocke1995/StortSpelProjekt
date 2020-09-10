#include "TestHeaderTwo.h"

TestHeaderTwo::TestHeaderTwo()
{
	EventBus::get().subscribe(this, &TestHeaderTwo::popMsg);
}

void TestHeaderTwo::popMsg(MessageLog* evnt)
{
	Log::Print("Printing from TestFileTwo, %s \n", evnt->msg);
}