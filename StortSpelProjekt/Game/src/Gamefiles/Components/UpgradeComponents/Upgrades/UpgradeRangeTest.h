#ifndef UPGRADERANGETEST_H
#define UPGRADERANGETEST_H
#include "Upgrade.h"

class Entity;

class UpgradeRangeTest : public Upgrade
{
public: 
	UpgradeRangeTest(Entity* parent);
	~UpgradeRangeTest();

	void OnHit();


private:

};

#endif
