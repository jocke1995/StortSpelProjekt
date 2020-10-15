#include "UpgradeRangeTest.h"
#include "ECS/Entity.h"
#include "Components/HealthComponent.h"

UpgradeRangeTest::UpgradeRangeTest(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeTest");
	// set the upgrade type/types!
	SetType(F_UpgradeType::RANGE | F_UpgradeType::PLAYER);
	// set the price of this upgrade
	m_Price = 8;

	// level starts with value 1
	m_AccelerationSpeed = 1000 * m_Level;
	// How much health to add.
	m_HealthChange = 100;
	// Setting direction to directly upwards.
	m_Direction.x = 0;
	m_Direction.y = 1;
	m_Direction.z = 0;
}

UpgradeRangeTest::~UpgradeRangeTest()
{
}

void UpgradeRangeTest::IncreaseLevel()
{
	m_Level++;
	m_AccelerationSpeed = 1000 * m_Level;
	ApplyStat();
}

void UpgradeRangeTest::OnRangedHit()
{
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationDirection(m_Direction);
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationSpeed(m_AccelerationSpeed);
}

void UpgradeRangeTest::ApplyStat()
{
	if (m_pParentEntity->HasComponent<component::HealthComponent>())
	{
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(m_HealthChange);
	}
}
