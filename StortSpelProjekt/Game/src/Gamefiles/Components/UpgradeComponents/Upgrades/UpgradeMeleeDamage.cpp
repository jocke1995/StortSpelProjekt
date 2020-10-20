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
	m_Price = 5;
	m_StartingPrice = m_Price;
	// Write a description for the upgrade
	m_Description = "Gives the player a damage boost of 2 at level 1. \nAt level 2 this boost is now 4, (intial boost of 2 multiplied by level), which is added on top of the previous 2 so 6 total damage boost. \nSo for levels 1-5 the total damage increase will be: 2, 2+4, 6+6, 12+8, 20+10 and so on.";

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
	m_DamageChange = m_StartingDamageChange * m_Level;
	// multiply starting price by level
	// price at level 10 vill be 5 * 10 = 50
	m_Price = m_StartingPrice * m_Level;
	// now apply the new stat
	ApplyStat();
}
