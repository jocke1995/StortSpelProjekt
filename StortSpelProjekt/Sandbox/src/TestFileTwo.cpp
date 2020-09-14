#include "TestFileTwo.h"

TestFile::TestFile()
{
	EventBus::GetInstance().Subscribe(this, &TestFile::CalledFunction);
}

TestFile::~TestFile()
{
	Log::Print("I am now destroyed \n");
}

void TestFile::unsub()
{
	EventBus::GetInstance().Unsubscribe(this, &TestFile::CalledFunction);
}

void TestFile::CalledFunction(TestEvent * evnt)
{
	Log::Print("REEEEE IT WORKS \n %d \n", evnt->number);
}