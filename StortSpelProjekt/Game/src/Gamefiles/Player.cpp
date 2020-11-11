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
	Entity* gamePlayer = SceneManager::GetInstance().GetScene("GameScene")->GetEntity("player");
	gamePlayer->GetComponent<component::CurrencyComponent>()->SetBalance(0);
	gamePlayer->GetComponent<component::UpgradeComponent>()->RemoveAllUpgrades();
	m_pUpgradeManager->RemoveAllUpgrades();
	// Values copied from main.
	component::MeleeComponent* mec = gamePlayer->GetComponent<component::MeleeComponent>();
	mec->SetDamage(10);
	mec->SetAttackInterval(0.8);
	component::RangeComponent* rac = gamePlayer->GetComponent<component::RangeComponent>();
	rac->SetDamage(10);
	rac->SetVelocity(50);
	rac->SetAttackInterval(0.8);

	Entity* shopPlayer = SceneManager::GetInstance().GetScene("GameScene")->GetEntity("player");
	shopPlayer->GetComponent<component::CurrencyComponent>()->SetBalance(0);
	shopPlayer->GetComponent<component::UpgradeComponent>()->RemoveAllUpgrades();


	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("money")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("0", "money");
	SceneManager::GetInstance().GetScene("ShopScene")->GetEntity("player")->GetComponent<component::CurrencyComponent>()->SetBalance(0);
	SceneManager::GetInstance().GetScene("ShopScene")->GetEntity("money")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("0", "money");
	// not suited here but otherwise it would be a loose function in main.
	QuadManager* man = SceneManager::GetInstance().GetScene("GameScene")->GetEntity("healthbar")->GetComponent<component::GUI2DComponent>()->GetQuadManager();
	man->UpdateQuad(man->GetPos(), { 0.275f, 0.055f }, false, false,
		{ 1.0, 1.0, 1.0, 1.0 },
		float3{ 0.0f, 1.0f, 0.0f });

	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("player")->GetComponent<component::HealthComponent>()->SetHealth(50);
	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("player")->GetComponent<component::HealthComponent>()->SetMaxHealth(50);
	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("healthBackground")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("50", "currentHealth");
	SceneManager::GetInstance().GetScene("GameScene")->GetEntity("healthBackground")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("50", "maxHealth");
}