#include "EngineMath.h"
#include "ECS/Entity.h"
#include "GameGUI.h"
#include "ECS/Scene.h"
#include "Player.h"

GameGUI::GameGUI()
{
	m_OldCurrHealth = 0;
	m_OldMoney = 0;
	m_HealthSize = { 0.275f, 0.055f };
}

void GameGUI::Update(double dt, Scene* scene)
{
	updateHealth(scene);

	Entity* entity = scene->GetEntity("money");
	if (entity != nullptr)
	{
		if (entity->HasComponent<component::GUI2DComponent>())
		{
			int money = Player::GetInstance().GetPlayer()->GetComponent<component::CurrencyComponent>()->GetBalace();
			if (money != m_OldMoney)
			{
				entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(
					"Currency: " + std::to_string(money),
					"money");
				m_OldMoney = money;
			}
		}
	}
}

void GameGUI::updateHealth(Scene* scene)
{
	Entity* entity = scene->GetEntity("healthBackground");
	if (entity != nullptr)
	{
		if (entity->HasComponent<component::GUI2DComponent>())
		{
			int health = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetHealth();
			int maxHealth = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetMaxHealth();

			if (health != m_OldCurrHealth)
			{
				component::GUI2DComponent* healthbar = scene->GetEntity("healthbar")->GetComponent<component::GUI2DComponent>();

				if (healthbar != nullptr)
				{
					// Percent of maxhealth
					float percent = static_cast<float>(health) / static_cast<float>(maxHealth);
					float2 size = { m_HealthSize.x * percent, m_HealthSize.y };

					// Coloring
					float3 color = { 0.0f, 0.0f, 0.0f };
					if (percent > 0.8f)
					{
						// Green
						color = { 0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
					}
					else if (percent > 0.6f)
					{
						// Lime
						color = { 127.5f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
					}
					else if (percent > 0.4f)
					{
						// Yellow
						color = { 255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
					}
					else if (percent > 0.2f)
					{
						// Orange
						color = { 255.0f / 255.0f, 127.5f / 255.0f, 0.0f / 255.0f };
					}
					else if (percent > 0.0f)
					{
						// Orange
						color = { 255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f };
					}

					healthbar->GetQuadManager()->UpdateQuad(
						healthbar->GetQuadManager()->GetPos(),
						size,
						false, false,
						healthbar->GetQuadManager()->GetAmountOfBlend(),
						color);
				}

				entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(std::to_string(health), "currentHealth");
				m_OldCurrHealth = health;
			}

			if (maxHealth != m_OldMaxHealth)
			{
				entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(std::to_string(maxHealth), "maxHealth");
				m_OldMaxHealth = maxHealth;
			}
		}
	}
}
