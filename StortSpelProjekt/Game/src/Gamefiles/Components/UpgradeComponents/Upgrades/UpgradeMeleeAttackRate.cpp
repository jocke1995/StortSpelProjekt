#include "UpgradeMeleeAttackRate.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeAttackRate::UpgradeMeleeAttackRate(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeAttackRate");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "default.png";
	m_upgradeFactor = 0;
}

UpgradeMeleeAttackRate::~UpgradeMeleeAttackRate()
{
}

void UpgradeMeleeAttackRate::ApplyStat()
{
	//TODO
}

void UpgradeMeleeAttackRate::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeMeleeAttackRate::IncreaseLevel()
{
	m_Level++;
	m_Price = m_StartingPrice * m_Level;
}
