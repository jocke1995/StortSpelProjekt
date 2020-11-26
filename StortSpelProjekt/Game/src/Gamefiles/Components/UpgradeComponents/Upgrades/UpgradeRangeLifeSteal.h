#ifndef UPGRADERANGELIFESTEAL_H
#define UPGRADERANGELIFESTEAL_H
#include "Upgrade.h"

class Entity;

// Upgrade description: This upgrade gives the player life steal based on ranged attack damage. First level gives 10 percent, next levels increase 5 percent per level.  Price doubles per level.
class UpgradeRangeLifeSteal : public Upgrade
{
public:
	UpgradeRangeLifeSteal(Entity* parent);
	~UpgradeRangeLifeSteal();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnRangedHit(Entity* target, Entity* projectile);
	void ApplyBoughtUpgrade();

	std::string GetDescription(unsigned int level);

private:
	float m_PercentageGain;

};

#endif
