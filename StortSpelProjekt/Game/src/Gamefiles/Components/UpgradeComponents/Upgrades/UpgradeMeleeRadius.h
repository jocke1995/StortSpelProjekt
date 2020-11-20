#ifndef UPGRADE_MELEE_RADIUS_H
#define UPGRADE_MELEE_RADIUS_H
#include "Upgrade.h"

class UpgradeMeleeRadius : public Upgrade
{
public:
	UpgradeMeleeRadius(Entity* parent);
	~UpgradeMeleeRadius();

	void ApplyBoughtUpgrade();
	void ApplyStat();
	void IncreaseLevel();

	std::string GetDescription(unsigned int level);

private:


};

#endif
