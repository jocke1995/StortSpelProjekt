#include "TestingFiles.h"

void TestingFiles::TestFunction(TestEvent* evnt)
{
	Log::Print("Reeeee");
}

TestingFiles::TestingFiles()
{
	EventBus::GetInstance().Subscribe(this, &TestingFiles::TestFunction);
}

TestingFiles::~TestingFiles()
{
}
