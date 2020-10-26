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

void Player::IsInShop(bool value)
{
	m_IsInShop = value;
}

// TODO: When Shop has UI remove this way of getting upgrades
void Player::buyUpgrade(UForUpgrade* evnt)
{
	if (m_IsInShop)
	{
		m_pShop->ApplyUppgrade("UpgradeRangeLifeSteal");
		Log::Print("UpgradeRangeLifeSteal applied!\n");
		m_pShop->ApplyUppgrade("UpgradeRangeVelocity");
		Log::Print("UpgradeRangeVelocity applied!\n");
		m_pShop->ApplyUppgrade("UpgradeMeleeDamage");
		Log::Print("UpgradeMeleeDamage applied!\n");
		m_pShop->ApplyUppgrade("UpgradeHealthBoost");
		Log::Print("UpgradeHealthBoost applied!\n");
	}
}
