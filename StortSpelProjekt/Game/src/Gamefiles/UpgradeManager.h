#ifndef UPGRADE_MANAGER_H
#define UPGRADE_MANAGER_H

#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
#include <map>
#include <string>
#include <vector>

// Include all upgrades
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeTest.h"


enum E_RangeName
{
	RANGE_TEST = 1,

};

class UpgradeManager
{
public:
	UpgradeManager(Entity* parentEntity);
	~UpgradeManager();
	

	void ApplyUpgrade(std::string name);
	void ApplyRangeUpgrades(Entity* ent);

private:
	Entity* m_pParentEntity = nullptr;
	// Contains all uppgrades that the player can buy
	std::map<std::string, Upgrade*> m_AllAvailablePlayerUpgrades;
	// Contains the name of all uppgrades that will be applied to porjectile Entities
	std::vector<std::string> m_AllAppliedProjectileUpgrades;
	// Contains enums for all rangeUpgrades. 
	// Used when getting NEW range upgrades for projectile entities.
	std::map<std::string, int> m_RangeUpgradeEnmus;

	// Populates m_AllAvailablePlayerUpgrades with all upgrades
	void fillUppgradeList();
	// Returns true if it is of type RANGE and should then be put on the Projectile entity
	bool checkIfRangeUpgrade(std::string name);
	// returns true if it is of a type that should be applied on the player entity
	bool checkIfPlayerEntityUpgrade(std::string name);
	// Returns a new rangeUpgrade so that every projectile entity has it's own rangeUpgrade.
	Upgrade* RangeUpgrade(std::string name, Entity* ent);
};


#endif // !UPGRADE_MANAGER_H