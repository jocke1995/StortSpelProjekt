#include "UpgradeMeleeRadius.h"

UpgradeMeleeRadius::UpgradeMeleeRadius(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeRadius");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "MeleeDamage.png";

}

UpgradeMeleeRadius::~UpgradeMeleeRadius()
{
}

void UpgradeMeleeRadius::ApplyBoughtUpgrade()
{

}

void UpgradeMeleeRadius::ApplyStat()
{
}

void UpgradeMeleeRadius::IncreaseLevel()
{
}

std::string UpgradeMeleeRadius::GetDescription(unsigned int level)
{
	return std::string();
}
