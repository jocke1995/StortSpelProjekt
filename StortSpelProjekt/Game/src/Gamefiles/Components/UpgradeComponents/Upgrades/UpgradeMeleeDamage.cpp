#include "UpgradeMeleeDamage.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeDamage::UpgradeMeleeDamage(Entity* parentEntity) : Upgrade(parentEntity)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeDamage");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 150;
	m_StartingPrice = m_Price;

	m_DamageChange = 25;

	m_ImageName = "MeleeDamage.png";
}

UpgradeMeleeDamage::~UpgradeMeleeDamage()
{

}

void UpgradeMeleeDamage::ApplyStat()
{
	m_pParentEntity->GetComponent<component::MeleeComponent>()->ChangeDamage(m_DamageChange);
}

void UpgradeMeleeDamage::IncreaseLevel()
{
	// increase level
	m_Level++;

	m_Price = m_StartingPrice * (m_Level + 1);
}

void UpgradeMeleeDamage::ApplyBoughtUpgrade()
{
	ApplyStat();
}

std::string UpgradeMeleeDamage::GetDescription(unsigned int level)
{
	return "Melee Damage: Improves the Melee damage by 50%, for a total of: " + std::to_string(50*level) + "\%";
}
