#pragma once
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
class UpgradeMeleeDamage : public Upgrade
{
public:
	UpgradeMeleeDamage(Entity* parentEntity);
	~UpgradeMeleeDamage();

	void ApplyStat();

	void IncreaseLevel();

private:
	int m_DamageChange;
	int m_StartingDamageChange;
	int m_StartingPrice;
};

