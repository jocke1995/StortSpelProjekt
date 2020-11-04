#include "EngineMath.h"
#include "ECS/Entity.h"
#include "GameGUI.h"
#include "ECS/Scene.h"
#include "Player.h"

GameGUI::GameGUI()
{
	m_OldHealth = 0;
	m_OldMoney = 0;
}

void GameGUI::Update(double dt, Scene* scene)
{
	Entity* entity = scene->GetEntity("health");
	if (entity != nullptr)
	{
		if (entity->HasComponent<component::GUI2DComponent>())
		{
			int health = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetHealth();
			if (health != m_OldHealth)
			{
				int maxHealth = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetMaxHealth();
				entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(
					std::to_string(health) + "/" + std::to_string(maxHealth),
					"health");
				m_OldHealth = health;
			}
		}
	}
	entity = scene->GetEntity("money");
	if (entity != nullptr)
	{
		if (entity->HasComponent<component::GUI2DComponent>())
		{
			int money = Player::GetInstance().GetPlayer()->GetComponent<component::CurrencyComponent>()->GetBalace();
			if (money != m_OldMoney)
			{
				entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(
					std::to_string(money),
					"money");
				m_OldMoney = money;
			}
		}
	}
}
