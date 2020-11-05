#include "EngineMath.h"
#include "ECS/Entity.h"
#include "GameGUI.h"
#include "ECS/Scene.h"
#include "Player.h"

GameGUI::GameGUI()
{
	m_OldMaxHealth = 0;
	m_OldCurrHealth = 0;
	m_OldMoney = 0;
	m_OldHealthLength = 0.0f;
	m_OldScene = nullptr;
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

	if (scene != m_OldScene)
	{
		reset(scene);
	}

	m_OldScene = scene;
}

void GameGUI::updateHealth(Scene* scene)
{
	Entity* entity = scene->GetEntity("healthBackground");
	if (entity != nullptr)
	{
		if (entity->HasComponent<component::GUI2DComponent>())
		{
			int health = scene->GetEntity("player")->GetComponent<component::HealthComponent>()->GetHealth();
			int maxHealth = scene->GetEntity("player")->GetComponent<component::HealthComponent>()->GetMaxHealth();
			component::GUI2DComponent* healthbar = scene->GetEntity("healthbar")->GetComponent<component::GUI2DComponent>();

			if (health != m_OldCurrHealth && healthbar != nullptr)
			{
				// Percentage of max health
				float percentage = static_cast<float>(health) / static_cast<float>(maxHealth);
				float2 size = healthbar->GetQuadManager()->GetScale();
				
				if (m_OldHealthLength != 0.0f)
				{
					size.x = m_OldHealthLength * percentage;
				}
				else
				{
					size.x *= percentage;
				}

				// Coloring
				float3 color = { 0.0f, 0.0f, 0.0f };
				if (percentage > 0.8f)
				{
					// Green
					color = { 0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
				}
				else if (percentage > 0.6f)
				{
					// Lime
					color = { 127.5f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
				}
				else if (percentage > 0.4f)
				{
					// Yellow
					color = { 255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
				}
				else if (percentage > 0.2f)
				{
					// Orange
					color = { 255.0f / 255.0f, 127.5f / 255.0f, 0.0f / 255.0f };
				}
				else if (percentage > 0.0f)
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

				// Always keep oldHealthLength at 100% of the original length
				m_OldHealthLength = size.x + (m_OldHealthLength - m_OldHealthLength * percentage);

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

void GameGUI::reset(Scene* scene)
{
	component::GUI2DComponent* healthbar = scene->GetEntity("healthbar")->GetComponent<component::GUI2DComponent>();
	float2 size = healthbar->GetQuadManager()->GetScale();
	size.x = m_OldHealthLength;
	float3 color = { 0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
	healthbar->GetQuadManager()->UpdateQuad(
		healthbar->GetQuadManager()->GetPos(),
		size,
		false, false,
		healthbar->GetQuadManager()->GetAmountOfBlend(),
		color);

	m_OldCurrHealth = 0;
	m_OldMaxHealth = 0;
	m_OldMoney = 0;
	m_OldHealthLength = 0;
}