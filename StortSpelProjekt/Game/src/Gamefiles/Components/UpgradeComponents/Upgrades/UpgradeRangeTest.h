#ifndef UPGRADERANGETEST_H
#define UPGRADERANGETEST_H
#include "Upgrade.h"
#include "EngineMath.h"

class Entity;

// IMPORTANT: Write a clear description of what this upgrade is meant to do, and how stacking/increasing its level is meant to work
// This upgrade is a test-upgrade with examples on how to implement your own upgrades.

// Upgrade description:
// Level change:
class UpgradeRangeTest : public Upgrade
{
public: 
	UpgradeRangeTest(Entity* parent);
	~UpgradeRangeTest();

	void IncreaseLevel();
	void OnHit();
	void OnRangedHit();
	void ApplyStat();


private:
	int m_AccelerationSpeed;
	float3 m_Direction;
	int m_HealthChange;

};

#endif
