#ifndef UPGRADERANGEVELOCITY_H
#define UPGRADERANGEVELOCITY_H
#include "Upgrade.h"
#include "EngineMath.h"

class Entity;

// IMPORTANT: Write a clear description of what this upgrade is meant to do, and how stacking/increasing its level is meant to work
// This upgrade is simply a test-upgrade with examples on how to implement your own upgrades.
// Note that this is a "RANGE" type, which is kind of a special case because the projectiles are separate entities (not player entity),
// this is something to keep in mind when making new upgrades, because m_pParentEntity will not always be the player. 

// Upgrade description: This upgrade is a test-upgrade which gives the player 100 extra hp, and makes the projectiles fly up to the sky when hitting a target.
// Level change: Increasing level makes the "accelerationspeed" up to the sky multiply with the level of the upgrade. 
// Healthchange stays the same at 100. So every level will add 100 health.

// Upgrade description: This upgrade increases the velocity of the range attacks
class UpgradeRangeVelocity : public Upgrade
{
public:
	UpgradeRangeVelocity(Entity* parent);
	~UpgradeRangeVelocity();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void ApplyStat();
	void ApplyBoughtUpgrade();

private:
	// Member varibles to be used for this upgrade
	int m_ChangeFactor;
	int m_DamageChange;
	std::string m_Description;

};






#endif
