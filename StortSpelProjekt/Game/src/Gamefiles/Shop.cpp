#include "Shop.h"
#include <time.h>
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/CurrencyComponent.h"

Shop::Shop()
{
	m_pUpgradeManager = Player::GetInstance().GetUpgradeManager();
	m_pPlayer = Player::GetInstance().GetPlayer();
	m_AllAvailableUpgrades = m_pUpgradeManager->GetAllAvailableUpgrades();
	for (auto upgrade : m_AllAvailableUpgrades)
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
		// While loop is neccessary to avoid duplicate upgrades in inventory.
		do
		{
			// need to reset this bool every loop
			inStock = false;
			// Get a random number that will be used to get an upgrade to the inventory
			upgradeNum = m_Rand.Rand(0, m_AllAvailableUpgrades.size());
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
		m_Prices[name] = m_AllAvailableUpgrades[name]->GetPrice();
		m_UpgradeDescriptions[name] = m_AllAvailableUpgrades[name]->GetDescription();
	}


}

void Shop::ApplyUppgrade(std::string name)
{
	if (checkExisting(name))
	{
		// Increasing UpgradeManagers m_AppliedUpgradeLevel level as well as upgradeComponents m_AppliedUpgrades 
		// because we want to increase level of RANGE type upgrades as well and this needs to be done in UpgradeManager.
		m_pPlayer->GetComponent<component::UpgradeComponent>()->GetUpgradeByName(name)->IncreaseLevel();
		m_pUpgradeManager->IncreaseLevel(name);
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

void Shop::SetPlayerBalance(int newBalance)
{
	m_pPlayer->GetComponent<component::CurrencyComponent>()->SetBalance(newBalance);
}

void Shop::ChangePlayerBalance(int change)
{
	m_pPlayer->GetComponent<component::CurrencyComponent>()->ChangeBalance(change);
}

int Shop::GetInventorySize()
{
	return m_InvSize;
}

std::vector<std::string> Shop::GetInventoryNames()
{
	return m_InventoryNames;
}

std::map<std::string, std::string> Shop::GetUpgradeDescriptions()
{
	return m_UpgradeDescriptions;
}

std::string Shop::GetUpgradeDescription(std::string name)
{
	return m_UpgradeDescriptions[name];
}

std::map<std::string, int> Shop::GetPrices()
{
	return m_Prices;
}

int Shop::GetPrice(std::string name)
{
	return m_Prices[name];
}

int Shop::GetPlayerBalance()
{
	return m_pPlayer->GetComponent<component::CurrencyComponent>()->GetBalace();
}

void Shop::clearInventory()
{
	m_InventoryNames.clear();
	m_UpgradeDescriptions.clear();
}

bool Shop::checkExisting(std::string name)
{
	return m_pUpgradeManager->IsUpgradeApplied(name);
}
