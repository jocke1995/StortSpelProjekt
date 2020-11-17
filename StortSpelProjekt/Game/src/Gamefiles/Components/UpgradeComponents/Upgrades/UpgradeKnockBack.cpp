#include "UpgradeKnockBack.h"

UpgradeKnockBack::UpgradeKnockBack(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeKnockBack");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "default.png";
	m_KnockBack = 5;
}

UpgradeKnockBack::~UpgradeKnockBack()
{
}

void UpgradeKnockBack::ApplyStat()
{
}

void UpgradeKnockBack::ApplyBoughtUpgrade()
{
}

void UpgradeKnockBack::IncreaseLevel()
{
}
