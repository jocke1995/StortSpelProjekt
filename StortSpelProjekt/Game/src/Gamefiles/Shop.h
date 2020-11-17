#ifndef SHOP_H
#define SHOP_H

#include "Player.h"
#include "UpgradeManager.h"
class UpgradeComponent;
class Upgrade;

#include "../Misc/EngineRand.h"
#include "../Events/Events.h"

class Texture;
class Font;

struct shopGUIStateChange;

class Shop
{
public:
	Shop();
	~Shop();

	// creates the 2d gui for the shop
	void Create2DGUI();
	// clear 2D GUI, removes entities
	void Clear2DGUI();
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
	// Get price of an upgrade using its name as input
	int GetPrice(std::string name);
	// Get player currency balance
	int GetPlayerBalance();
	// Get upgrade image
	Texture* GetUpgradeImage(std::string* name);
	// Get if the user is inside the 2D shop GUI
	bool IsShop2DGUIDisplaying();
	// Resets the inventory of the shop.
	void Reset();

	// Event that happens when user presses F
	void OnShopGUIStateChange(shopGUIStateChange* collisionEvent);

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

	// randomize buyable upgrades
	void randomizeInventory();
	// clears the inventory vector as well as the m_UpgradeDescriptions map
	void clearInventory();
	// checks if an upgrade is already bought
	bool checkExisting(std::string name);

	Font* m_pArial = nullptr;

	bool m_DisplayingShopGUI = false;
};

#endif // !SHOP_H