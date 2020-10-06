#ifndef UPGRADECOMPONENT_H
#define UPGRADECOMPONENT_H
#include "ECS/Components/Component.h"
#include <vector>
#include <string>

enum E_Type
{
	RANGE = 0,			// range weapon/attack upgrades
	MELEE = 1,			// melee weapon/attack upgrades
	MOVEMENT = 2,		// upgrades regarding movement.
	STATS = 3,			// stat modifications such as hp change.
	ONDAMAGE = 4,		// upgrades that are "triggered" when damage is taken.
	ENEMYSPECIFIC = 5	// upgrades only relevant for enemy entities. example: enemy explode on death)
};

namespace component
{
	class UpgradeComponent : public Component
	{
	public:
		UpgradeComponent(Entity* parent, std::string name);
		virtual ~UpgradeComponent();
		virtual void Update(double dt);
		virtual void RenderUpdate(double dt);

	private:
		std::string m_Name;
		int m_Level;
		E_Type m_Type;

	};
}

#endif
