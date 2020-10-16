#include "UpgradeMeleeTest.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeTest::UpgradeMeleeTest(Entity* parentEntity) : Upgrade(parentEntity)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeTest");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 3;

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
