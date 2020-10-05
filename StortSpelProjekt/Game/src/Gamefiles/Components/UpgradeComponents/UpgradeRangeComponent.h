#ifndef UPGRADERANGECOMPONENT_H
#define UPGRADERANGECOMPONENT_H
#include "UpgradeComponent.h"

namespace component
{
	class UpgradeRangeComponent : public UpgradeComponent
	{
	public:
		UpgradeRangeComponent(Entity* parent);
		virtual ~UpgradeRangeComponent();
		virtual void RangedHit();

	private:

	};
}


#endif
