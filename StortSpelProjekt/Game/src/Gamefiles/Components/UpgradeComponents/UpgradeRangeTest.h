#ifndef UPGRADERANGETEST_H
#define UPGRADERANGETEST_H

#include "UpgradeRangeComponent.h"

namespace component
{
	class UpgradeRangeTest : public UpgradeRangeComponent
	{
	public: 
		UpgradeRangeTest(Entity* parent, std::string name);
		~UpgradeRangeTest();

		void OnHit();
		void RangedFlight();
	};
}

#endif
