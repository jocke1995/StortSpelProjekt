#include "Player.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "UpgradeManager.h"
#include "Shop.h"
#include "Events/EventBus.h"
#include "ECS/SceneManager.h"

Player::Player()
{
	m_pPlayer = nullptr;
	m_pShop = nullptr;
	m_pUpgradeManager = nullptr;
	EventBus::GetInstance().Subscribe(this, &Player::onResetGame);
}

Player& Player::GetInstance()
{
	static Player instance;
	return instance;
}

Player::~Player()
{
	EventBus::GetInstance().Unsubscribe(this, &Player::onResetGame);
	delete m_pUpgradeManager;
	delete m_pShop;
}

void Player::SetPlayer(Entity* player)
{
	m_pPlayer = player;
	m_pUpgradeManager = new UpgradeManager(player);
	m_pShop = new Shop;
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

void Player::onResetGame(ResetGame* evnt)
{
	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("player")->GetComponent<component::CurrencyComponent>()->SetBalance(0);
	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("money")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("0", "money");
	SceneManager::GetInstance().GetScene("ShopScene")->GetEntity("player")->GetComponent<component::CurrencyComponent>()->SetBalance(0);
	SceneManager::GetInstance().GetScene("ShopScene")->GetEntity("money")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("0", "money");
}