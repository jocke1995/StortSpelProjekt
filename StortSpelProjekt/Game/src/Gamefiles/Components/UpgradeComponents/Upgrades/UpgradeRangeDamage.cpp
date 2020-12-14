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
	m_Price = 150;
	m_StartingPrice = m_Price;

	m_DamageChange = 20;

	m_ImageName = "RangeUpgrade.png";
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

	m_Price = m_StartingPrice * (m_Level + 1);
	// now apply the new stat
}

void UpgradeRangeDamage::ApplyBoughtUpgrade()
{
	ApplyStat();;
}

std::string UpgradeRangeDamage::GetDescription(unsigned int level)
{
	return "Range Damage: Improves the Range damage by 50%, for a total of: " + std::to_string(50*level) + "\%";
}
