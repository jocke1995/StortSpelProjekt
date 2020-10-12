#ifndef UPGRADE_MELEE_TEST_H
#define UPGRADE_MELEE_TEST_H

#include "Upgrade.h"
#include "Components/MeleeComponent.h"

class Entity;

// IMPORTANT: Write a clear description of what this upgrade is meant to do, and how stacking/increasing its level is meant to work
// This upgrade is simply a test-upgrade with examples on how to implement your own upgrades.

// Upgrade description: This upgrade is a test-upgrade which gives the player 2 extra melee damage.
// Level change: Increasing level multiplies the m_DamageChange with level. So will apply 2 then 4 then 6 and so on. 
// Example of how the melee damage output will scale wtih this upgrade: lvl1: 1+2=3, lvl2: 3+4=7, lvl3: 7+6=13 and so on.

class UpgradeMeleeTest : public Upgrade
{
public:
	UpgradeMeleeTest(Entity* parentEntity);
	~UpgradeMeleeTest();

	void ApplyStat();

	// Multiplies 
	void IncreaseLevel();

private:
	int m_DamageChange;
};


#endif // !UPGRADE_MELEE_TEST_H