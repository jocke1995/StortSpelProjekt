#include "Player.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

Player::Player()
{
	// fillUppgradeList();
}

Player& Player::GetInstance()
{
	static Player instance;
	return instance;
}

Player::~Player()
{
	//for (auto upgrades : m_AllAvailablePlayerUpgrades)
	//{
	//	delete upgrades.second;
	//}
	//for (auto upgrades : m_AllAvailablePlayerProjectileUpgrades)
	//{
	//	delete upgrades.second;
	//}
	//for (auto upgrades : m_AllAppliedProjectileUpgrades)
	//{
	//	delete upgrades.second;
	//}
	//m_AllAvailablePlayerUpgrades.clear();
	//m_AllAvailablePlayerProjectileUpgrades.clear();
	//m_AllAppliedProjectileUpgrades.clear();
}

void Player::SetPlayer(Entity* player)
{
	m_pPlayer = player;
	fillUppgradeList();
}

Entity* Player::GetPlayer()
{
	return m_pPlayer;
}

void Player::fillUppgradeList()
{
	Upgrade* upgrade;

	// Adding RangeTest Upgrade
	upgrade = new UpgradeRangeTest(m_pPlayer);
	// add the upgrade to the list of all upgrades
	m_AllAvailablePlayerUpgrades[upgrade->GetName()] = upgrade;


	// Add the upgrade to the list of all projectile upgrades.
	// The entity is sent as nullptr because it will be set to the projectile entity later.
	// Must be new and not the same pointer as the one
	// TODO: Fix this shit!! Needs to be a new entity every time a projectile gets shot out. 
	// Potential fix is to have a switch case with enmus and getting the new range upgrade from there.
	// Don't forget to merg in andres push
	// prolly not needed anymore, m_AllAvailablePlayerProjectileUpgrades.push_back(upgrade->GetName());


	//upgrade = new Upgrade...Test(m_pPlayer);
	//m_AllAvailablePlayerUpgrades[upgrade->GetName()] = upgrade;
}

void Player::ApplyUpgrade(std::string name)
{
	if (checkIfRangeUpgrade(name))
	{
		// If it is an uppgrade that needs to be put on a projectile then
		// add it to the m_AllAppliedProjectileUpgrades list
		m_AllAppliedProjectileUpgrades.push_back(name);
		m_RangeUpgradeEnmus[name] = RANGE_TEST;
		// Then check if it is also of a type that needs to be on the player entity.
		// If so also add it to player entitys UpgradeComponent
		if (checkIfPlayerEntityUpgrade(name))
		{
			m_pPlayer->GetComponent<component::UpgradeComponent>()->AddUpgrade(m_AllAvailablePlayerUpgrades[name]);
		}
	}
	else
	{
		// If it is not a Range Upgrade then it goes on the player entity.
		// So add it to player entitys UpgradeComponent
		m_pPlayer->GetComponent<component::UpgradeComponent>()->AddUpgrade(m_AllAvailablePlayerUpgrades[name]);
	}
}

void Player::ApplyRangeUpgrades(Entity* ent)
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

bool Player::checkIfRangeUpgrade(std::string name)
{
	if (F_UpgradeType::RANGE & m_AllAvailablePlayerUpgrades[name]->GetType())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Player::checkIfPlayerEntityUpgrade(std::string name)
{
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

Upgrade* Player::RangeUpgrade(std::string name, Entity* ent)
{
	
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

