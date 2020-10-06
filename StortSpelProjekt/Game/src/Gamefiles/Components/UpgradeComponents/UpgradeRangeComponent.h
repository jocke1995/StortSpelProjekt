#ifndef UPGRADERANGECOMPONENT_H
#define UPGRADERANGECOMPONENT_H
#include "UpgradeComponent.h"

namespace component
{
	class UpgradeRangeComponent : public UpgradeComponent
	{
	public:
		UpgradeRangeComponent(Entity* parent, std::string name);
		virtual ~UpgradeRangeComponent();
		// upgrades that are triggered on hit (ex: explosive, poison)
		virtual void OnHit();
		// upgrades that affect the flight pattern (ex: gravity, speed)
		virtual void RangedFlight();
		//// upgrades that modify the ranged attack (ex: multiple projectiles)
		//virtual void RangedModifier();

	private:

	};
}


#endif
