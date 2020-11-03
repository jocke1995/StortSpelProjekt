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
	m_Price = 100;
	m_StartingPrice = m_Price;
	// Write a description for the upgrade
	m_Description = "Melee Damage: Improves the Melee damage by +2";

	m_DamageChange = 2;
	m_StartingDamageChange = m_DamageChange;

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
	// multiply the damageChange at level 1 with current level
	// damage change by level 10 vill be 20
	// total change to damage at level 10 will be 20 + 80 = 100
	//m_DamageChange = m_StartingDamageChange * m_Level;
	// multiply starting price by level
	// price at level 10 vill be 5 * 10 = 50
	m_Price = m_StartingPrice + m_StartingPrice * (m_Level / 2);
	// now apply the new stat
	ApplyStat();
}

void UpgradeMeleeDamage::ApplyBoughtUpgrade()
{
	ApplyStat();
	m_Price = m_StartingPrice * m_Level;
}
