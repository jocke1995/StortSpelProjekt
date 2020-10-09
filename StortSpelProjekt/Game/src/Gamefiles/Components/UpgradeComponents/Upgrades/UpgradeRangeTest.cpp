#include "UpgradeRangeTest.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeRangeTest::UpgradeRangeTest(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeTest");
	// set the upgrade type
	SetType(F_UpgradeType::RANGE);
	// increase upgrade level
	IncreaseLevel();
}

UpgradeRangeTest::~UpgradeRangeTest()
{
}

void UpgradeRangeTest::OnHit()
{
	Log::Print("UpgradeRangeTest OnHit called\n");
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationDirection(0, 1, 0);
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationSpeed(1000);
}

