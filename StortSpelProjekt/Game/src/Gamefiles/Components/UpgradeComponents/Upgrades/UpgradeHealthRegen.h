#ifndef UPGRADE_HEALTH_REGEN_H
#define UPGRADE_HEALTH_REGEN_H

#include "Components/UpgradeComponents/Upgrades/Upgrade.h"

class UpgradeHealthRegen : public Upgrade
{
public:
	UpgradeHealthRegen(Entity* parentEntity);
	~UpgradeHealthRegen();

	void Update(double dt);

	void IncreaseLevel();

	std::string GetDescription(unsigned int level);

private:
	double m_HealthTimer;

};

#endif