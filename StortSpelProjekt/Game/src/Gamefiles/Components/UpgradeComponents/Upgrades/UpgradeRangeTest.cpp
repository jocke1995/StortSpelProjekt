#include "UpgradeRangeTest.h"
//#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/HealthComponent.h"

UpgradeRangeTest::UpgradeRangeTest(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeTest");
	// set the upgrade type
	SetType(F_UpgradeType::RANGE | F_UpgradeType::STATS);

	// level starts with value 1
	m_AccelerationSpeed = 1000 * m_Level;
	m_HealthChange = 100;
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

void UpgradeRangeTest::OnHit()
{
}

void UpgradeRangeTest::OnRangedHit()
{
	Log::Print("UpgradeRangeTest OnRangedHit called\n");
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationDirection(m_Direction);
	m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationSpeed(m_AccelerationSpeed);
}

void UpgradeRangeTest::ApplyStat()
{
	if (m_pParentEntity->HasComponent<component::HealthComponent>())
	{
		int oldHealth = m_pParentEntity->GetComponent<component::HealthComponent>()->GetHealth();
		Log::Print("Old Health: %d\n", oldHealth);
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(m_HealthChange);
		int newHealth = m_pParentEntity->GetComponent<component::HealthComponent>()->GetHealth();
		Log::Print("New Health: %d\n", newHealth);
	}
}

