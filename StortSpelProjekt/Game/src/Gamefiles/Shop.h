#ifndef SHOP_H
#define SHOP_H

#include "Player.h"
#include "UpgradeManager.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
#include <string>
#include "Misc/EngineRand.h"


class Shop
{
public:
	Shop();
	~Shop();

	// randomize buyable upgrades
	void RandomizeInventory();
	// add uppgrade to player
	void ApplyUppgrade(std::string name);
	// Set the size of inventory
	void SetInventorySize(int size);

	// Get size of inventory
	int GetInventorySize();
	// Get vector of buyable upgrades names.
	std::vector<std::string> GetInventoryNames();
	// Get descriptions, for upgrades in inventory, mapped to their names.
	std::map<std::string, std::string> GetUpgradeDiscriptions();
	// Get discription for an upgrade in inventory using its name as input.
	std::string GetUpgradeDiscription(std::string name);
	// Get prices of upgrades in inventory
	int GetPrices();


private:
	// upgradeManager
	UpgradeManager* m_pUpgradeManager = nullptr;
	// all upgrades mapped to their names
	std::map<std::string, Upgrade*> m_AllAvailableUppgrades;
	// vector of all upgrades names
	std::vector<std::string> m_UpgradeNames;
	// vector of names of the upgrades in inventory
	std::vector<std::string> m_InventoryNames;
	// inventory size
	int m_InvSize;
	// Used to randomize the inventory
	EngineRand m_Rand;
	// Contains the description of all upgrades currently in the inventory
	std::map<std::string, std::string> m_UpgradeDescriptions;
	// list of all UI-components
	// list of buyable uppgrades maped to names
	std::map<std::string, Upgrade*> m_Inventory;
	//// cost of uppgrades mapped to names
	//std::

	// clears the inventory vector as well as the m_UpgradeDescriptions map
	void clearInventory();
	// checks if an upgrade is already bought
	bool checkExisting(std::string name);


};

#endif // !SHOP_H