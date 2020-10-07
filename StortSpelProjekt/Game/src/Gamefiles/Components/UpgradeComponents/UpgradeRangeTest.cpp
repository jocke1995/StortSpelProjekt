#include "UpgradeRangeTest.h"
#include "stdafx.h"
#include "../ECS/Entity.h"

component::UpgradeRangeTest::UpgradeRangeTest(Entity* parent)
	:UpgradeComponent(parent)
{
	SetName("UpgradeRangeTest");
	SetType(E_Type::RANGE);
	IncreaseLevel();
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

