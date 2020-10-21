#include "Player.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "UpgradeManager.h"
#include "Shop.h"
#include "Events/EventBus.h"

Player::Player()
{
	m_pPlayer = nullptr;
	m_pShop = nullptr;
	m_pUpgradeManager = nullptr;
}

Player& Player::GetInstance()
{
	static Player instance;
	return instance;
}

Player::~Player()
{
	delete m_pUpgradeManager;
	delete m_pShop;
}

void Player::SetPlayer(Entity* player)
{
	m_pPlayer = player;
	m_pUpgradeManager = new UpgradeManager(player);
	m_pShop = new Shop;

	EventBus::GetInstance().Subscribe(this, &Player::buyUpgrade);
}

Entity* Player::GetPlayer()
{
	return m_pPlayer;
}

UpgradeManager* Player::GetUpgradeManager()
{
	return m_pUpgradeManager;
}

Shop* Player::GetShop()
{
	return m_pShop;
}

// TODO: When Shop has UI remove this way of getting upgrades
void Player::buyUpgrade(UForUpgrade* evnt)
{
	Shop* shop = Player::GetInstance().GetShop();
	if (m_UpgradeApplied % 2 == 0)
	{
		shop->ApplyUppgrade("UpgradeHealthBoost");
		Log::Print("UpgradeHealthBoost applied!\n");
	}
	else
	{
		shop->ApplyUppgrade("UpgradeMeleeDamage");
		Log::Print("UpgradeMeleeTest applied!\n");
	}
	m_UpgradeApplied++;
}
