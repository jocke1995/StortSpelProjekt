#ifndef UPGRADECOMPONENT_H
#define UPGRADECOMPONENT_H
#include "ECS/Components/Component.h"
#include <vector>
#include <string>

namespace component
{
	class UpgradeComponent : public Component
	{
	public:
		UpgradeComponent(Entity* parent, std::string name);
		virtual ~UpgradeComponent();
		virtual void Update(double dt);
		virtual void RenderUpdate(double dt);

		// range
		// melee
		// movement
		// stats
		// onDamage
		
		// fiendespecifika uppgr (ex explosion on death)


	private:
		std::string m_Name;
		int m_Level;



	};
}



#endif
