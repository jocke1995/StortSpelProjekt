#ifndef UPGRADERANGELIFESTEAL_H
#define UPGRADERANGELIFESTEAL_H
#include "Upgrade.h"

class Entity;

// Upgrade description: 
class UpgradeRangeLifeSteal : public Upgrade
{
public:
	UpgradeRangeLifeSteal(Entity* parent);
	~UpgradeRangeLifeSteal();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnRangedHit();


private:
	float m_PercentageGain;


};

#endif

