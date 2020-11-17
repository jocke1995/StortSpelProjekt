#include "UpgradeManager.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
// Include all uppgrades under here:
#include "Components/UpgradeComponents/Upgrades/UpgradeMeleeDamage.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeTest.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeVelocity.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeLifeSteal.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeHealthBoost.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeBlueJewel.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeBounce.h"
#include "Components/UpgradeComponents/Upgrades/UpgradePoisonAttack.h"
#include "Components/UpgradeComponents/Upgrades/UpgradeMeleeAttackRate.h"

UpgradeManager::UpgradeManager(Entity* parentEntity)
{
	m_pParentEntity = parentEntity;
	// After parent entity is set we fill in our map of upgrades.
	fillUpgradeMap();

	// Fill in the names of all upgrades in m_AppliedUpgradeLevel and set their level to level 1
	for (auto u : m_AllAvailableUpgrades)
	{
		m_AppliedUpgradeLevel[u.first] = 0;
	}
}

UpgradeManager::~UpgradeManager()
{
	for (std::pair<std::string, Upgrade*> u : m_AllAvailableUpgrades)
	{
		delete u.second;
	}
}

void UpgradeManager::ApplyUpgrade(std::string name)
{
	// Adds this to the enum map that should contain all applied upgrades.
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

			m_pParentEntity->GetComponent<component::UpgradeComponent>()->AddUpgrade(newUpgrade(name, m_pParentEntity));
		}
	}
	else
	{
		// If it is not a Range Upgrade then it goes on the player entity.
		// So add it to player entitys UpgradeComponent.
		m_pParentEntity->GetComponent<component::UpgradeComponent>()->AddUpgrade(newUpgrade(name, m_pParentEntity));
	}
}

void UpgradeManager::ApplyRangeUpgrades(Entity* ent)
{
	Upgrade* rangeUpgrade;
	// Run through all range upgrades that the player has bought.
	for (auto upgradeName : m_AllAppliedProjectileUpgrades)
	{
		// get NEW RangeUpgrade for the projectile entity
		rangeUpgrade = newUpgrade(upgradeName, ent);
		// Check if the upgrade has increased in level. 
		// If so increase level of the new upgrade so it matches the one in m_AppliedUpgradeLevel
		// i = 1, because upgrades start at level 1.
		int upgradeLevel = m_AppliedUpgradeLevel[upgradeName];
		for (int i = 0; i < upgradeLevel; i++)
		{
			rangeUpgrade->IncreaseLevel();
		}
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

std::map<std::string, Upgrade*> UpgradeManager::GetAllAvailableUpgrades()
{
	return m_AllAvailableUpgrades;
}

void UpgradeManager::IncreaseLevel(std::string name)
{
	m_AppliedUpgradeLevel[name]++;
	m_AllAvailableUpgrades[name]->IncreaseLevel();
}

std::map<std::string, int> UpgradeManager::GetAppliedUpgradesLevel()
{
	return m_AppliedUpgradeLevel;
}

void UpgradeManager::RemoveAllUpgrades()
{
	m_AllAppliedProjectileUpgrades.clear();
	m_AppliedUpgradeEnums.clear();
	m_AppliedUpgradeLevel.clear();
}

std::map<std::string, int> UpgradeManager::GetAppliedUpgradeEnums()
{
	return m_AppliedUpgradeEnums;
}

void UpgradeManager::fillUpgradeMap()
{
	Upgrade* upgrade;

	// Adding RangeVelocity Upgrade - The range velocity is set on rangecomponent which goes on the player
	upgrade = new UpgradeRangeVelocity(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_RANGE_VELOCITY);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding Range lifesteal Upgrade
	upgrade = new UpgradeRangeLifeSteal(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_RANGE_LIFESTEAL);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding MeleeDamage Upgrade
	upgrade = new UpgradeMeleeDamage(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_MELEE_DAMAGE);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding HealthBoost Upgrade
	upgrade = new UpgradeHealthBoost(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_HEALTH_BOOST);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding BlueJewel Upgrade
	upgrade = new UpgradeBlueJewel(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_BLUE_JEWEL);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding RangeBounce Upgrade
	upgrade = new UpgradeRangeBounce(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_RANGE_BOUNCE);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding PoisonAttack Upgrade
	upgrade = new UpgradePoisonAttack(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_POISON_ATTACK);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	// Adding MeleeAttackRate Upgrade
	upgrade = new UpgradeMeleeAttackRate(m_pParentEntity);
	// Set upgrade ID to the appropriate enum in E_UpgradeIDs
	upgrade->SetID(UPGRADE_MELEE_ATTACKRATE);
	// add the upgrade to the list of all upgrades
	m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;
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

Upgrade* UpgradeManager::newUpgrade(std::string name, Entity* ent)
{
	// Using the enum that is mapped to name,
	// return the correct NEW upgrade with parentEntity ent
	switch (m_AppliedUpgradeEnums[name])
	{
	case UPGRADE_MELEE_DAMAGE:
		return new UpgradeMeleeDamage(ent);
		break;
	case UPGRADE_HEALTH_BOOST:
		return new UpgradeHealthBoost(ent);
		break;
	case UPGRADE_RANGE_VELOCITY:
		return new UpgradeRangeVelocity(ent);
		break;
	case UPGRADE_RANGE_LIFESTEAL:
		return new UpgradeRangeLifeSteal(ent);
		break;
	case UPGRADE_BLUE_JEWEL:
		return new UpgradeBlueJewel(ent);
		break;
	case UPGRADE_RANGE_BOUNCE:
		return new UpgradeRangeBounce(ent);
		break;
	case UPGRADE_POISON_ATTACK:
		return new UpgradePoisonAttack(ent);
		break;
	case UPGRADE_MELEE_ATTACKRATE:
		return new UpgradeMeleeAttackRate(ent);
		break;
	default:
		break;
	}
}

