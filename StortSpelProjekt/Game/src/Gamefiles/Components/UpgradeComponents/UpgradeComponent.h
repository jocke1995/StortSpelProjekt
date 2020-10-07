#ifndef UPGRADECOMPONENT_H
#define UPGRADECOMPONENT_H
#include "ECS/Components/Component.h"
#include <vector>
#include <string>

// Types of upgrades, for sorting/call purposes
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
		UpgradeComponent(Entity* parent, std::string name, int type);
		virtual ~UpgradeComponent();
		virtual void Update(double dt);
		virtual void RenderUpdate(double dt);

		// Below are all functions needed by upgrades. Some will be used by several upgrades and others might be unique.
		// This way you can call functions that range over several "types" but might be called on in similar situations (such as melee/range OnHit).
		// Declarations are to be implemented in the separate upgrade component classes. Add more as needed.

		// upgrades that are triggered on hit (ex: explosive, poison)
		virtual void OnHit();
		//// upgrades that affect the flight pattern (ex: gravity, speed)
		//virtual void RangedFlight();
		//// upgrades that modify the ranged attack (ex: multiple projectiles)
		//virtual void RangedModifier();

	private:
		// Name of the upgrade-component, for ease of access in shop or upgrade handlers
		std::string m_Name;
		// What level the upgrade is, for keeping track of stacking of upgrades and price
		int m_Level;
		// What type of upgrade this is, for ease of sorting or making calls to certain types
		int m_Type;

	};
}

#endif
