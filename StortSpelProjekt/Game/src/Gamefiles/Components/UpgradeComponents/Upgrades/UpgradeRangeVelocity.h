#ifndef UPGRADERANGEVELOCITY_H
#define UPGRADERANGEVELOCITY_H
#include "Upgrade.h"
#include "EngineMath.h" // might not be needed

class Entity;

class UpgradeRangeVelocity : public Upgrade
{
public:
	UpgradeRangeVelocity(Entity* parent);
	~UpgradeRangeVelocity();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void RangedFlight();
	void ApplyStat();
	void ApplyBoughtUpgrade();

private:
	// Member varibles to be used for this upgrade
	int m_ChangeFactor;
	int m_DamageChange;

};






#endif
