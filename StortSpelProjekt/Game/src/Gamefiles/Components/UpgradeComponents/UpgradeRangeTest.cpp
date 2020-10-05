#include "UpgradeRangeTest.h"
#include "stdafx.h"

component::UpgradeRangeTest::UpgradeRangeTest(Entity* parent)
	:UpgradeRangeComponent(parent)
{
}

component::UpgradeRangeTest::~UpgradeRangeTest()
{
}

void component::UpgradeRangeTest::RangedHit()
{
	Log::Print("UpgradeRangeTest called\n");
}
