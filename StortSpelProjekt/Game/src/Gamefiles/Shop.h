#ifndef SHOP_H
#define SHOP_H

#include "Player.h"
#include "UpgradeManager.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
#include <string>
#include "Misc/EngineRand.h"
#include "Events/Events.h"

class Texture;

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
	// Set player currencyComponents currency
	void SetPlayerBalance(int newBalance);
	// Change player currency balance
	void ChangePlayerBalance(int change);

	// Get size of inventory
	int GetInventorySize();
	// Get vector of buyable upgrades names.
	std::vector<std::string> GetInventoryNames();
	// Get descriptions, for upgrades in inventory, mapped to their names.
	std::map<std::string, std::string> GetUpgradeDescriptions();
	// Get description for an upgrade in inventory using its name as input.
	std::string GetUpgradeDescription(std::string name);
	// Get prices of upgrades in inventory
	std::map<std::string, int> GetPrices();
	// Get price of an upgrade using its name as input
	int GetPrice(std::string name);
	// Get player currency balance
	int GetPlayerBalance();
	// Get upgrade image
	Texture* GetUpgradeImage(std::string* name);


private:
	void upgradePressed(ButtonPressed* evnt);
	void sceneChange(SceneChange* evnt);

	// upgradeManager
	UpgradeManager* m_pUpgradeManager = nullptr;
	// Player entity
	Entity* m_pPlayer;
	// all upgrades mapped to their names
	std::map<std::string, Upgrade*> m_AllAvailableUpgrades;
	// vector of all upgrades names
	std::vector<std::string> m_UpgradeNames;
	// vector of names of the upgrades in inventory
	std::vector<std::string> m_InventoryNames;
	// vector to check if inventory has been bought
	std::vector<bool> m_InventoryIsBought;
	// inventory size
	int m_InvSize;
	// Used to randomize the inventory
	EngineRand m_Rand;
	// Contains the description of all upgrades currently in the inventory
	std::map<std::string, std::string> m_UpgradeDescriptions;
	// cost of uppgrades mapped to names
	std::map<std::string, int> m_Prices;

	// clears the inventory vector as well as the m_UpgradeDescriptions map
	void clearInventory();
	// checks if an upgrade is already bought
	bool checkExisting(std::string name);


};

#endif // !SHOP_H