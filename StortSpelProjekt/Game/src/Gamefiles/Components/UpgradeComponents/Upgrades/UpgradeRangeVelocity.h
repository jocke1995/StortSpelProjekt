#ifndef UPGRADERANGEVELOCITY_H
#define UPGRADERANGEVELOCITY_H
#include "Upgrade.h"

class Entity;

// Upgrade description: This upgrade increases the velocity of the range attacks with 10 per level. Max level 5 (velocity = 100). Price doubles per level.
class UpgradeRangeVelocity : public Upgrade
{
public:
	UpgradeRangeVelocity(Entity* parent);
	~UpgradeRangeVelocity();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void ApplyBoughtUpgrade();

private:
	// Member varibles to be used for this upgrade
	std::string m_Description;

};






#endif
