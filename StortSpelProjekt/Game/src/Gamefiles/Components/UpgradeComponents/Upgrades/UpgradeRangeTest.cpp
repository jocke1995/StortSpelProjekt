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
	// TODO: REMOVE THE PRINTS FROM THIS FUNCTION EFTER TESTING
	Log::Print("UpgradeRangeTest OnRangedHit called\n");
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationDirection(m_Direction);
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationSpeed(m_AccelerationSpeed);
}

void UpgradeRangeTest::ApplyStat()
{
	if (m_pParentEntity->HasComponent<component::HealthComponent>())
	{
		// TODO: REMOVE THE PRINTS AND GetHealth() FROM THIS FUNCTION AFTER TESTING
		int oldHealth = m_pParentEntity->GetComponent<component::HealthComponent>()->GetHealth();
		Log::Print("Old Health: %d\n", oldHealth);
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(m_HealthChange);
		int newHealth = m_pParentEntity->GetComponent<component::HealthComponent>()->GetHealth();
		Log::Print("New Health: %d\n", newHealth);
	}
}
