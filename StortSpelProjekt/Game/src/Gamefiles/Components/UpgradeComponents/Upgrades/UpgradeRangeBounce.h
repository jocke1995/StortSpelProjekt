#ifndef UPGRADE_RANGE_BOUNCE_H
#define UPGRADE_RANGE_BOUNCE_H

#include "Components/UpgradeComponents/Upgrades/Upgrade.h"

class UpgradeRangeBounce : public Upgrade
{
public:
	UpgradeRangeBounce(Entity* parentEntity);
	~UpgradeRangeBounce();

	void IncreaseLevel();
	void ApplyBoughtUpgrade();
	std::string GetDescription(unsigned int level);

private:

};

#endif // !UPGRADE_RANGE_BOUNCE_H