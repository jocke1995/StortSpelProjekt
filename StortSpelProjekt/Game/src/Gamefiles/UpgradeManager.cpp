#include "UpgradeManager.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeTest.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeMeleeTest.h"

UpgradeManager::UpgradeManager(Entity* parentEntity)
{
	m_pParentEntity = parentEntity;
	// After parent entity is set we fill in our map of upgrades.
	fillUpgradeMap();
}

UpgradeManager::~UpgradeManager()
{

}

void UpgradeManager::ApplyUpgrade(std::string name)
{
	// Adds this to the enum map of that should contain all applied upgrades.
	m_AppliedUpgradeEnums[name] = m_AllAvailableUpgrades[name]->GetID();

	if (checkIfRangeUpgrade(name))
	{
		// If it is an uppgrade that needs to be put on a projectile then
		// add it to the m_AllAppliedProjectileUpgrades vector.
		m_AllAppliedProjectileUpgrades.push_back(name);

		// Then check if it is also of a type that needs to be on the player entity.
		// If so also add it to player entitys UpgradeComponent.
		if (checkIfPlayerEntityUpgrade(name))
		{
			m_pParentEntity->GetComponent<component::UpgradeComponent>()->AddUpgrade(m_AllAvailableUpgrades[name]);
		}
	}
	else
	{
		// If it is not a Range Upgrade then it goes on the player entity.
		// So add it to player entitys UpgradeComponent.
		m_pParentEntity->GetComponent<component::UpgradeComponent>()->AddUpgrade(m_AllAvailableUpgrades[name]);
	}
}

void UpgradeManager::ApplyRangeUpgrades(Entity* ent)
{
	Upgrade* rangeUpgrade;
	// Run through all range upgrades that the player has bought.
	for (auto upgradeName : m_AllAppliedProjectileUpgrades)
	{
		// get NEW RangeUpgrade for the projectile entity
		rangeUpgrade = RangeUpgrade(upgradeName, ent);
		// Add the upgrade to the projectile entitys upgradeComponent
		ent->GetComponent<component::UpgradeComponent>()->AddUpgrade(rangeUpgrade);
	}
}

bool UpgradeManager::IsUpgradeApplied(std::string name)
{
	for (auto map : m_AppliedUpgradeEnums)
	{
		if (map.first == name)
		{
			return true;
		}
	}
	return false;
}

bool UpgradeManager::IsUpgradeApplied(int id)
{
	for (auto map : m_AppliedUpgradeEnums)
	{
		if (map.second == id)
		{
			return true;
		}
	}
	return false;
}

void UpgradeManager::fillUpgradeMap()
{
	Upgrade* upgrade;

	// Adding RangeTest Upgrade
	upgrade = new UpgradeRangeTest(m_pParentEntity);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;
	// Also, since it is of type RANGE, add its' Enum to the enum map.
	m_RangeUpgradeEnums[upgrade->GetName()] = UPGRADE_RANGE_TEST;
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_RANGE_TEST);

	// Adding MeleeTest Upgrade
	upgrade = new UpgradeMeleeTest(m_pParentEntity);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_MELEE_TEST);
}

bool UpgradeManager::checkIfRangeUpgrade(std::string name)
{
	// Checking if the upgrade with name, is a range upgrade
	if (F_UpgradeType::RANGE & m_AllAvailableUpgrades[name]->GetType())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UpgradeManager::checkIfPlayerEntityUpgrade(std::string name)
{
	// checking if upgrade with name, is of other type than RANGE
	if (F_UpgradeType::PLAYER & m_AllAvailableUpgrades[name]->GetType())
	{
		return true;
	}
	else
	{
		return false;
	}
}

Upgrade* UpgradeManager::RangeUpgrade(std::string name, Entity* ent)
{
	// Using the enum that is mapped to name,
	// return the correct NEW range upgrade with parentEntity ent
	switch (m_RangeUpgradeEnums[name])
	{
	case UPGRADE_RANGE_TEST:
		return new UpgradeRangeTest(ent);
		break;
	default:
		break;
	}
}