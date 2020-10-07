#include "UpgradeRangeTest.h"
#include "stdafx.h"
#include "../ECS/Entity.h"

component::UpgradeRangeTest::UpgradeRangeTest(Entity* parent, std::string name, int type)
	:UpgradeComponent(parent, name, type)
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

