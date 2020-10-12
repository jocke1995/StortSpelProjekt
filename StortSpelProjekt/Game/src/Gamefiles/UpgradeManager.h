#ifndef UPGRADE_MANAGER_H
#define UPGRADE_MANAGER_H

#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
#include <map>
#include <string>
#include <vector>

// Include all upgrades
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeTest.h"

// This enum is used when creating NEW Upgrades of type RANGE for projectile Entities.
// Need to add an enum for each upgrade that is of type RANGE.
// Please name the enums the same as the upgrades m_Name variable.
enum E_RangeName
{
	UPGRADE_RANGE_TEST = 1,

};

class UpgradeManager
{
public:
	UpgradeManager(Entity* parentEntity);
	~UpgradeManager();
	
	// Applies an upgrade to the entitys upgradeComponent.
	void ApplyUpgrade(std::string name);
	// Applies all bought upgrades of type RANGE on projectile entities in the RangeComponent.
	void ApplyRangeUpgrades(Entity* ent);

private:
	Entity* m_pParentEntity = nullptr;
	// Contains all uppgrades that the player can buy
	std::map<std::string, Upgrade*> m_AllAvailableUpgrades;
	// Contains the name of all uppgrades that will be applied to projectile Entities
	std::vector<std::string> m_AllAppliedProjectileUpgrades;
	// Contains enums for all rangeUpgrades. 
	// Used when creating NEW upgrades, of type RANGE, for projectile entities.
	std::map<std::string, int> m_RangeUpgradeEnmus;

	// Populates m_AllAvailableUpgrades with all upgrades
	void fillUppgradeMap();
	// Returns true if it is of type RANGE and should then be put on the Projectile entity
	bool checkIfRangeUpgrade(std::string name);
	// returns true if it is of a type that should be applied on the player entity
	bool checkIfPlayerEntityUpgrade(std::string name);
	// Returns a new Upgrade, of type RANGE, so that 
	// every projectile entity has it's own version of that Upgrade.
	Upgrade* RangeUpgrade(std::string name, Entity* ent);
};


#endif // !UPGRADE_MANAGER_H