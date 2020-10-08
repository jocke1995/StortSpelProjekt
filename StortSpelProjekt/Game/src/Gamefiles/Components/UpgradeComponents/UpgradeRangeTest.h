#ifndef UPGRADERANGETEST_H
#define UPGRADERANGETEST_H
#include "UpgradeComponent.h"

namespace component
{
	class UpgradeRangeTest : public UpgradeComponent, public Component
	{
	public: 
		UpgradeRangeTest(Entity* parent);
		~UpgradeRangeTest();

		void OnHit();
	};
}

#endif
