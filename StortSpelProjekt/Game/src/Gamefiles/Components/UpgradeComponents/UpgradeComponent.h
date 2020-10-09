#ifndef UPGRADECOMPONENT_H
#define UPGRADECOMPONENT_H
#include "ECS/Components/Component.h"
#include <string>
#include <map>

class Upgrade;
namespace component
{
	class UpgradeComponent : public Component
	{
	public:
		UpgradeComponent(Entity* parent);
		~UpgradeComponent();

		// Inherited functions from component, if dt is needed for an upgrade
		void Update(double dt);
		void RenderUpdate(double dt);

		// setters and getters for private member variables
		void SetName(std::string name);
		std::string GetName() const;
		void IncreaseLevel();
		void DecreaseLevel();
		int GetLevel() const;
		void SetType(int type);
		unsigned int GetType() const;
		//std::map<std::string, Upgrade*>* GetUpgrades() const;
		void AddUpgrade(Upgrade* upgrade);
		void RemoveUpgrade(Upgrade* upgrade);
		bool HasUpgrade(std::string name);

		// Below are all functions needed by upgrades. Some will be used by several upgrades and others might be unique.
		// This way you can call functions that range over several "types" but might be called on in similar situations (such as melee/range OnHit).
		// Declarations are to be implemented in the separate upgrade component classes. Add more as needed.

		// upgrades that are triggered on hit (ex: explosive, poison)
		void OnHit();
		// specific for ranged hits
		void OnRangedHit();
		// specific for melee hits
		void OnMeleeHit();
		// upgrades that are triggered when taking damage
		void OnDamage();
		// upgrades that are applied immediately, for example apply stat when bought in shop
		void OnPickUp();
		// upgrades triggered on death
		void OnDeath();
		// upgrades that affect the flight pattern (ex: gravity, speed)
		void RangedFlight();
		// upgrades that modify the ranged attack (ex: multiple projectiles)
		void RangedModifier();
		// upgrades that applies/adds stats (ex: more hp, more stamina)
		void ApplyStat();

	private:
		// list with all added upgrades to the parent entity
		std::map<std::string, Upgrade*> m_AppliedUpgrades;
		// Name of the upgrade-component, for ease of access in shop or upgrade handlers
		std::string m_Name;
		// What level the upgrade is, for keeping track of stacking of upgrades and price
		int m_Level = 0;
		// What type of upgrade this is, for ease of sorting or making calls to certain types
		unsigned int m_Type;

	};
}

#endif
