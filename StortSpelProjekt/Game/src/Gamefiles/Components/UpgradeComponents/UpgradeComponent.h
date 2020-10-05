#ifndef UPGRADECOMPONENT_H
#define UPGRADECOMPONENT_H
#include "ECS/Components/Component.h"
#include <vector>

namespace component
{
	class UpgradeComponent : public Component
	{
	public:
		UpgradeComponent(Entity* parent);
		virtual ~UpgradeComponent();
		virtual void Update(double dt);
		virtual void RenderUpdate(double dt);

		// these functions will be called by relevant upgrade components
		//virtual void ApplyUpgrade();
		//virtual void RangedFlight();
		//virtual void RangedModifier();
		virtual void RangedHit();



	private:
		std::vector<UpgradeComponent*> m_UpgradeComponents;

	};
}



#endif
