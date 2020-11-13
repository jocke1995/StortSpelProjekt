#ifndef UPGRADE_MELEE_DAMAGE_H
#define UPGRADE_MELEE_DAMAGE_H

#include "Components/UpgradeComponents/Upgrades/Upgrade.h"

class UpgradeMeleeDamage : public Upgrade
{
public:
	UpgradeMeleeDamage(Entity* parentEntity);
	~UpgradeMeleeDamage();

	void ApplyStat();
	void IncreaseLevel();
	void ApplyBoughtUpgrade();

	std::string GetDescription(unsigned int level);

private:
	int m_DamageChange;
};

#endif // !UPGRADE_MELEE_DAMAGE_H