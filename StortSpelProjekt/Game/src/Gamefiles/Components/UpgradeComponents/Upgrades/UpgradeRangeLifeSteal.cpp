#include "UpgradeRangeLifeSteal.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeRangeLifeSteal::UpgradeRangeLifeSteal(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeLifeSteal");
	// set the upgrade type/types!
	SetType(F_UpgradeType::RANGE);	// goes on the projectiles
	// set the price of this upgrade
	m_Price = 1;
	// set short description 
	m_Description = "steals da life";
	// percentage of damage done to steal as life
	m_PercentageGain = 0.05;
}

UpgradeRangeLifeSteal::~UpgradeRangeLifeSteal()
{
}

void UpgradeRangeLifeSteal::IncreaseLevel()
{
}

void UpgradeRangeLifeSteal::OnRangedHit()
{
	Log::Print("TESTPRINT POOPS\n");
}
