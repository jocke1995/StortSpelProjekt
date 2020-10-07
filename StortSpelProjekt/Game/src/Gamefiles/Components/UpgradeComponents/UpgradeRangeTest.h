#ifndef UPGRADERANGETEST_H
#define UPGRADERANGETEST_H
#include "UpgradeComponent.h"

namespace component
{
	class UpgradeRangeTest : public UpgradeComponent
	{
	public: 
		UpgradeRangeTest(Entity* parent, std::string name, int type);
		~UpgradeRangeTest();

		void OnHit();
	};
}

#endif
