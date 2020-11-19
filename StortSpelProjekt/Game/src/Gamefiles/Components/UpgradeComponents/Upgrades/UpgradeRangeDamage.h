#ifndef UPGRADE_RANGE_DAMAGE_H
#define UPGRADE_RANGE_DAMAGE_H

#include "Components/UpgradeComponents/Upgrades/Upgrade.h"

class UpgradeRangeDamage : public Upgrade
{
public:
	UpgradeRangeDamage(Entity* parentEntity);
	~UpgradeRangeDamage();

	void ApplyStat();
	void IncreaseLevel();
	void ApplyBoughtUpgrade();

	std::string GetDescription(unsigned int level);

private:
	int m_DamageChange;
};

#endif // !UPGRADE_RANGE_DAMAGE_H