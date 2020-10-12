#include "UpgradeManager.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeManager::UpgradeManager(Entity* parentEntity)
{
	m_pParentEntity = parentEntity;
	// After parent entity is set we fill in our map of upgrades.
	fillUppgradeList();
}

UpgradeManager::~UpgradeManager()
{

}

void UpgradeManager::ApplyUpgrade(std::string name)
{
	if (checkIfRangeUpgrade(name))
	{
		// If it is an uppgrade that needs to be put on a projectile then
		// add it to the m_AllAppliedProjectileUpgrades vector
		m_AllAppliedProjectileUpgrades.push_back(name);
		m_RangeUpgradeEnmus[name] = RANGE_TEST;
		// Then check if it is also of a type that needs to be on the player entity.
		// If so also add it to player entitys UpgradeComponent
		if (checkIfPlayerEntityUpgrade(name))
		{
			m_pParentEntity->GetComponent<component::UpgradeComponent>()->AddUpgrade(m_AllAvailablePlayerUpgrades[name]);
		}
	}
	else
	{
		// If it is not a Range Upgrade then it goes on the player entity.
		// So add it to player entitys UpgradeComponent
		m_pParentEntity->GetComponent<component::UpgradeComponent>()->AddUpgrade(m_AllAvailablePlayerUpgrades[name]);
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
		// Add the upgrade to the projectile entitys upgardeComponent
		ent->GetComponent<component::UpgradeComponent>()->AddUpgrade(rangeUpgrade);
	}
}

void UpgradeManager::fillUppgradeList()
{
	Upgrade* upgrade;

	// Adding RangeTest Upgrade
	upgrade = new UpgradeRangeTest(m_pParentEntity);
	// add the upgrade to the list of all upgrades
	m_AllAvailablePlayerUpgrades[upgrade->GetName()] = upgrade;

}

bool UpgradeManager::checkIfRangeUpgrade(std::string name)
{
	// checking if the upgrade with name, is a range upgrade
	if (F_UpgradeType::RANGE & m_AllAvailablePlayerUpgrades[name]->GetType())
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
	// checking if upgred with name, is of another type other than RANGE
	unsigned int type = m_AllAvailablePlayerUpgrades[name]->GetType();
	if (F_UpgradeType::MELEE & type || F_UpgradeType::MOVEMENT & type || F_UpgradeType::ONDAMAGE & type || F_UpgradeType::STATS & type)
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
	switch (m_RangeUpgradeEnmus[name])
	{
	case RANGE_TEST:
		return new UpgradeRangeTest(ent);
		break;
		//case:
		//	break;
	default:
		break;
	}
}