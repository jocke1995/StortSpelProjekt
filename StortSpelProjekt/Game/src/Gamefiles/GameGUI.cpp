#include "EngineMath.h"
#include "ECS/Entity.h"
#include "GameGUI.h"
#include "ECS/Scene.h"
#include "Player.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
#include "ECS/SceneManager.h"

GameGUI::GameGUI()
{
	m_OldMaxHealth = 0;
	m_OldHealth = 0;
	m_OldMoney = 0;
	m_OldHealthLength = 0.0f;
	m_pOldScene = nullptr;
	m_TimePlayed = 0;
	m_TimePlayedTimer = 0;

	EventBus::GetInstance().Subscribe(this, &GameGUI::sceneChange);
}

GameGUI& GameGUI::GetInstance()
{
	static GameGUI instance;
	return instance;
}


void GameGUI::Update(double dt, Scene* scene)
{
	updateHealth(scene);

	if (scene->GetName() == "GameScene" || scene->GetName() == "ShopScene")
	{
		//Count up for how long the player has played
		m_TimePlayedTimer += dt;
		if (m_TimePlayedTimer >= 1.0)
		{
			m_TimePlayedTimer -= 1.0;
			m_TimePlayed++;
		}

		//Update Currency
		if (scene->EntityExists("money"))
		{
			Entity* entity = scene->GetEntity("money");
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

	if (scene->GetName() == "OptionScene")
	{
		Entity* entity = nullptr;

		entity = scene->GetEntity("brightness");
		entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(Option::GetInstance().GetVariable("f_brightness"), "brightness");

		entity = scene->GetEntity("volume");
		entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(Option::GetInstance().GetVariable("f_volume"), "volume");

		entity = scene->GetEntity("MouseSensitivity");
		entity->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(Option::GetInstance().GetVariable("f_sensitivityX"), "MouseSensitivity");
	}

	if (scene != m_pOldScene)
	{
		reset(scene);
	}

	m_pOldScene = scene;
}

int GameGUI::GetTimePlayed()
{
	return m_TimePlayed;
}

void GameGUI::updateHealth(Scene* scene)
{
	if (scene->EntityExists("healthBackground"))
	{
		Entity* entity = scene->GetEntity("healthBackground");
		if (entity->HasComponent<component::GUI2DComponent>())
		{
			int health = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetHealth();
			int maxHealth = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetMaxHealth();
			component::GUI2DComponent* healthbar = scene->GetEntity("healthbar")->GetComponent<component::GUI2DComponent>();

			if (health != m_OldHealth && healthbar != nullptr)
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
					// Red
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
				m_OldHealth = health;
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
	if (scene->EntityExists("healthbar"))
	{
		component::GUI2DComponent* healthbar = scene->GetEntity("healthbar")->GetComponent<component::GUI2DComponent>();
		float2 size = healthbar->GetQuadManager()->GetScale();
		size.x = m_OldHealthLength;
		float3 color = { 0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
		healthbar->GetQuadManager()->UpdateQuad(
			healthbar->GetQuadManager()->GetPos(),
			{ 0.229f, 0.046f },
			false, false,
			healthbar->GetQuadManager()->GetAmountOfBlend(),
			color);
	}

	m_OldHealth = 0;
	m_OldMaxHealth = 0;
	m_OldMoney = 0;
	m_OldHealthLength = 0;
}

void GameGUI::sceneChange(SceneChange* evnt)
{
	//If the player enters Main Menu Scene then they have either just started the game or ended a game.
	if (evnt->m_NewSceneName == "MainMenuScene")
	{
		m_TimePlayed = 0;
		m_TimePlayedTimer = 0;
	}
}
