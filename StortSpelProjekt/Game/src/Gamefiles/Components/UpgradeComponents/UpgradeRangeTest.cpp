#include "UpgradeRangeTest.h"
#include "stdafx.h"
#include "../ECS/Entity.h"

component::UpgradeRangeTest::UpgradeRangeTest(Entity* parent, std::string name)
	:UpgradeRangeComponent(parent, name)
{	

}

component::UpgradeRangeTest::~UpgradeRangeTest()
{
}

void component::UpgradeRangeTest::OnHit()
{
	Log::Print("UpgradeRangeTest OnHit called\n");
	m_pParent->GetComponent<component::AccelerationComponent>()->SetAccelerationDirection(0, 1, 0);
	m_pParent->GetComponent<component::AccelerationComponent>()->SetAccelerationSpeed(1000);
}

void component::UpgradeRangeTest::RangedFlight()
{
	Log::Print("UpgradeRangeTest RangedFlight called\n");
}
