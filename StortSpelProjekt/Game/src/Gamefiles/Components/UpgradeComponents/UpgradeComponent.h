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

		// inherited pure virtual functions
		//SceneInit
		void OnInitScene();
		void OnUnInitScene();

		// setters and getters for private member variables
		void AddUpgrade(Upgrade* upgrade);
		void RemoveUpgrade(Upgrade* upgrade);
		bool HasUpgrade(std::string name);
		std::map<std::string, Upgrade*> GetUpgradeMap();
		Upgrade* GetUpgradeByName(std::string name);

		// Below are all functions needed by upgrades. Some will be used by several upgrades and others might be unique.
		// This way you can call functions that range over several "types" but might be called on in similar situations (such as melee/range OnHit).
		// These functions will call on the Upgrade class virtual functions, which are implemented in the specific upgrades classes.
		// Add more as needed.

		// upgrades that are triggered on hit (ex: explosive, poison)
		void OnHit(Entity* target);
		// specific for ranged hits
		void OnRangedHit(Entity* target);
		// specific for melee hits
		void OnMeleeHit(Entity* target);
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

	private:
		// list with all added upgrades to the parent entity
		std::map<std::string, Upgrade*> m_AppliedUpgrades;

	};
}

#endif
