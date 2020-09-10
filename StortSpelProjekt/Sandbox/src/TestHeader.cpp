#include "TestHeader.h"

TestSubscriber::TestSubscriber()
{
	EventBus::get().subscribe(this, &TestSubscriber::popMsg);
}

void TestSubscriber::popMsg(MessageLog * evnt)
{
	Log::Print("Printing from TestFileOne, %s \n", evnt->msg);
}
