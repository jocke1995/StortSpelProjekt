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


void Player::buyUpgrade(UForUpgrade* evnt)
{
	Shop* shop = Player::GetInstance().GetShop();
	shop->ApplyUppgrade("UpgradeRangeTest");
	Log::Print("UpgradeRangeTest applied!\n");
}
