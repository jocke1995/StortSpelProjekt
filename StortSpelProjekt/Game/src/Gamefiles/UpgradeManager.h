#ifndef UPGRADE_MANAGER_H
#define UPGRADE_MANAGER_H
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
#include <map>
#include <string>
#include <vector>

// forward declarations
class UpgradeComponent;
class UpgradeRangeTest;
class UpgradeMeleeTest;

// IMPORTANT: Need to add an enum for each upgrade. Will contain IDs for all upgrades.
// This enum is used in IsUpgradeApplied() to see if an upgrade is applied.
// It is also used when creating NEW Upgrades of type RANGE for projectile Entities.
// Please name the enums the same as the upgrades m_Name variable.
enum E_UpgradeIDs
{
	UPGRADE_RANGE_TEST = 1,
	UPGRADE_MELEE_TEST = 2,

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

	// returns true if the upgrade with name is in the map and therefore applied
	bool IsUpgradeApplied(std::string name);
	// returns true if the upgrade with id is in the map and therefore applied
	bool IsUpgradeApplied(int id);

	std::map<std::string, Upgrade*> GetAllAvailableUpgrades();

private:
	Entity* m_pParentEntity = nullptr;
	// Contains all upgrades that the player can buy
	std::map<std::string, Upgrade*> m_AllAvailableUpgrades;
	// Contains the name of all upgrades that will be applied to projectile Entities
	std::vector<std::string> m_AllAppliedProjectileUpgrades;
	// Contains enums for all applied upgrades. 
	// Useful when you want to check if an upgrade is applied or not
	std::map<std::string, int> m_AppliedUpgradeEnums;
	// Contains enums for all rangeUpgrades. 
	// Used when creating NEW upgrades, of type RANGE, for projectile entities.
	std::map<std::string, int> m_RangeUpgradeEnums;

	// Populates m_AllAvailableUpgrades with all upgrades and sets upgrade IDs in E_UpgradeIDs
	void fillUpgradeMap();
	// Returns true if it is of type RANGE and should then be put on the Projectile entity
	bool checkIfRangeUpgrade(std::string name);
	// returns true if it is of a type that should be applied on the player entity
	bool checkIfPlayerEntityUpgrade(std::string name);
	// Returns a new Upgrade, of type RANGE, so that 
	// every projectile entity has it's own version of that Upgrade.
	Upgrade* newRangeUpgrade(std::string name, Entity* ent);
};


#endif // !UPGRADE_MANAGER_H