#ifndef UPGRADERANGETEST_H
#define UPGRADERANGETEST_H
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
class UpgradeRangeTest : public Upgrade
{
public: 
	UpgradeRangeTest(Entity* parent);
	~UpgradeRangeTest();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnRangedHit();
	void ApplyStat();

private:
	// Member variables to be used for this upgrade
	int m_AccelerationSpeed;
	float3 m_Direction;
	int m_HealthChange;

};

#endif
