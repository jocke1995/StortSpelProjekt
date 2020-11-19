#include "UpgradeRangeDamage.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeRangeDamage::UpgradeRangeDamage(Entity* parentEntity) : Upgrade(parentEntity)
{
	// set the name of the upgrade
	SetName("UpgradeRangeDamage");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_DamageChange = 2;

	m_ImageName = "Rangedamage.png";
}

UpgradeRangeDamage::~UpgradeRangeDamage()
{

}

void UpgradeRangeDamage::ApplyStat()
{
	m_pParentEntity->GetComponent<component::RangeComponent>()->ChangeDamage(m_DamageChange);
}

void UpgradeRangeDamage::IncreaseLevel()
{
	// increase level
	m_Level++;

	m_Price = m_StartingPrice + m_StartingPrice * ((float)m_Level / 2.0f);
	// now apply the new stat
}

void UpgradeRangeDamage::ApplyBoughtUpgrade()
{
	ApplyStat();;
}

std::string UpgradeRangeDamage::GetDescription(unsigned int level)
{
	return "Range Damage: Improves the Range damage by +2, for a total of: " + std::to_string(2*level);
}
