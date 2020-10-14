#include "Shop.h"
#include <time.h>

Shop::Shop()
{
	m_pUpgradeManager = Player::GetInstance().GetUpgradeManager();
	m_AllAvailableUppgrades = m_pUpgradeManager->GetAllAvailableUpgrades();
	for (auto upgrade : m_AllAvailableUppgrades)
	{
		m_UpgradeNames.push_back(upgrade.first);
	}
	m_Rand = EngineRand(time(NULL));
	m_InvSize = 2;
}

Shop::~Shop()
{

}

void Shop::RandomizeInventory()
{
	// start with clearing the inventory so we don't get the same upgrades as before.
	clearInventory();
	int upgradeNum;
	std::string name;
	bool inStock = false;
	// Fill our inventory
	for (int i = 0; i < m_InvSize; i++)
	{
		// Set the seed for randomization of inventory
		m_Rand.SetSeed(time(NULL));
		do
		{
			// need to reset this bool every loop
			inStock = false;
			// Get a random number that will be used to get an upgrade to the inventory
			upgradeNum = m_Rand.Rand(0, m_AllAvailableUppgrades.size());
			// Take this number to get a name from m_UpgradeNames, which contains all avalible upgrades
			name = m_UpgradeNames.at(upgradeNum);
			for (auto names : m_InventoryNames)
			{
				if (name == names)
				{
					// If this name already exists in our inventory, 
					// then it is "inStock" so we have to try again.
					inStock = true;
					break;
				}
			}

		} while (inStock);
		// When we get an upgrade that was not in already in our inventory,
		// we add it to the inventory.
		m_InventoryNames.push_back(name);
		m_Inventory[name] = m_AllAvailableUppgrades[name];
		m_UpgradeDescriptions[name] = m_AllAvailableUppgrades[name]->GetDescription();
	}


}

void Shop::ApplyUppgrade(std::string name)
{
	if (checkExisting(name))
	{
		// Using m_AllAvailableUppgrades instead of upgradeComponents m_AppliedUpgrades 
		// because we want to increase level of RANGE type upgrades as well.
		m_AllAvailableUppgrades[name]->IncreaseLevel();
	}
	else
	{
		m_pUpgradeManager->ApplyUpgrade(name);
	}
}

void Shop::SetInventorySize(int size)
{
	m_InvSize = size;
}

int Shop::GetInventorySize()
{
	return m_InvSize;
}

std::vector<std::string> Shop::GetInventoryNames()
{
	return m_InventoryNames;
}

std::map<std::string, std::string> Shop::GetUpgradeDiscriptions()
{
	return m_UpgradeDescriptions;
}

std::string Shop::GetUpgradeDiscription(std::string name)
{
	return m_UpgradeDescriptions[name];
}

void Shop::clearInventory()
{
	m_Inventory.clear();
	m_InventoryNames.clear();
	m_UpgradeDescriptions.clear();
}

bool Shop::checkExisting(std::string name)
{
	return m_pUpgradeManager->IsUpgradeApplied(name);
}
