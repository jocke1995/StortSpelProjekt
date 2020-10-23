#ifndef UPGRADE_HEALTH_BOOST_H
#define UPGRADE_HEALTH_BOOST_H

#include "Components/UpgradeComponents/Upgrades/Upgrade.h"

class UpgradeHealthBoost : public Upgrade
{
public:
	UpgradeHealthBoost(Entity* parentEntity);
	~UpgradeHealthBoost();

	void ApplyStat();
	void IncreaseLevel();
	void ApplyBoughtUpgrade();

private:
	int m_HealthBoost;

};

#endif // !UPGRADE_HEALTH_BOOST_H