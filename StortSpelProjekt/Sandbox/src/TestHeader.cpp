#include "TestHeader.h"

TestSubscriber::TestSubscriber()
{
	EventBus::GetInstance().Subscribe(this, &TestSubscriber::popMsg);
}

void TestSubscriber::popMsg(MessageLog * evnt)
{
	Log::Print("Printing from TestFileOne, %s \n", evnt->msg.c_str());
}
