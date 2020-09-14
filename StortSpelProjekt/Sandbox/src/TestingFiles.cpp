#include "TestingFiles.h"

TestingFiles::TestingFiles()
{
	EventBus::GetInstance().Subscribe(this, &TestingFiles::TestFunction);
}

TestingFiles::~TestingFiles()
{
}

void TestingFiles::TestFunction(EventType * evnt)
{
}
