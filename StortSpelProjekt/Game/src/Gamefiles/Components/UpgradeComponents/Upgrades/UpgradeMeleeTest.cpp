#include "UpgradeMeleeTest.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeTest::UpgradeMeleeTest(Entity* parentEntity) : Upgrade(parentEntity)
{
	SetName("UpgradeMeleeTest");
	SetType(F_UpgradeType::PLAYER);
	m_DamageChange = 2;
}

UpgradeMeleeTest::~UpgradeMeleeTest()
{

}

void UpgradeMeleeTest::ApplyStat()
{
	m_pParentEntity->GetComponent<component::MeleeComponent>()->ChangeDamage(m_DamageChange);
}

void UpgradeMeleeTest::IncreaseLevel()
{
	m_Level++;
	m_DamageChange *= m_Level;
	ApplyStat();
}
