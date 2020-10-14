#ifndef SHOP_H
#define SHOP_H

#include "Player.h"
#include "UpgradeManager.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Components/UpgradeComponents/Upgrades/Upgrade.h"
#include <string>

class Shop
{
public:
	Shop();
	~Shop();

	// randomize buyable upgrades
	void RandomizeInventory();
	// add uppgrade to list of all uppgrades
	void AddUpgrade();
	// add uppgrade to player
	void ApplyUppgrade(std::string name);
	// get list of buyable upgrades

private:
	// upgradeManager
	UpgradeManager* m_pUpgradeManager = nullptr;
	// list of all upgrades maped to names
	std::vector<std::string> m_UpgradeNames;
	// inventory size
	int m_InvSize;
	// list of all UI-components
	// list of buyable uppgrades maped to names
	// cost of uppgrades mapped to names

	void checkExisting(std::string name);

};

#endif // !SHOP_H