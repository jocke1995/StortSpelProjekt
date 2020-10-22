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

private:
	int m_DamageChange;
	int m_StartingDamageChange;
};

#endif // !UPGRADE_MELEE_DAMAGE_H