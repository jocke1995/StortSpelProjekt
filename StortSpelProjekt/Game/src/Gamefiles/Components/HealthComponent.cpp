#include "HealthComponent.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "ECS/Entity.h"
#include "UpgradeComponents/UpgradeComponent.h"
#include "Player.h"
#include "Misc/AssetLoader.h"

component::HealthComponent::HealthComponent(Entity* parent, int hp, float removalTime) : Component(parent)
{
	m_FlatDamageReduction = 0;
	m_MultiplicativeDamageReduction = 1.0f;
	m_RemovalTimer = removalTime;
	m_Health = hp;
	// set max health to same as hp arg when created
	m_MaxHealth = m_Health;

	m_100 = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/EnemyHealth100.png");
	m_80 = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/EnemyHealth80.png");
	m_60 = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/EnemyHealth60.png");
	m_40 = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/EnemyHealth40.png");
	m_20 = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/EnemyHealth20.png");
}

component::HealthComponent::~HealthComponent()
{

}

void component::HealthComponent::Update(double dt)
{
	m_DeathDuration += static_cast<double>(m_Dead * dt);
	if (m_RemovalTimer <= m_DeathDuration && m_Dead)
	{
		if (m_pParent->GetName() != "player")
		{
			EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
		}
		else
		{
			EventBus::GetInstance().Publish(&SceneChange("gameOverScene"));
			// reset death timer
			m_DeathDuration = 0.0f;
		}
	}
}

void component::HealthComponent::OnInitScene()
{
}

void component::HealthComponent::OnUnInitScene()
{
}

void component::HealthComponent::SetHealth(int hp)
{
	m_Health = hp;
	component::ProgressBarComponent* pc = m_pParent->GetComponent<component::ProgressBarComponent>();
	if (pc != nullptr)
	{
		pc->EnableProgressBar();
		pc->SetProgressBarPercent(static_cast<float>(m_Health) / static_cast<float>(m_MaxHealth));
	}
	if (m_Health <= 0 && m_Dead == false)
	{
		m_Dead = true;
		component::CollisionComponent* comp = m_pParent->GetComponent<component::CollisionComponent>();
		if (comp)
		{
			comp->SetAngularFactor({ 1,1,1 });
			comp->SetAngularVelocity(5, 0, 0);
		}
		if (m_pParent->GetName() == "Player")
		{
			Player::GetInstance().SetPlayerAlive(false);
		}
		EventBus::GetInstance().Publish(&Death(m_pParent));
	}
}

void component::HealthComponent::ChangeHealth(int hpChange)
{
	m_Health += hpChange * static_cast<float>(m_Health > 0);
	if (m_Health <= 0 && m_Dead == false)
	{
		m_Health = 0;
		m_Dead = true;
		component::CollisionComponent* comp = m_pParent->GetComponent<component::CollisionComponent>();
		if (comp)
		{
			comp->SetAngularFactor({ 1,1,1 });
			comp->SetAngularVelocity(5,0,0);
		}
		EventBus::GetInstance().Publish(&Death(m_pParent));
	}

	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}

	component::ProgressBarComponent* pc = m_pParent->GetComponent<component::ProgressBarComponent>();
	if (pc != nullptr)
	{
		pc->EnableProgressBar();
		float percentage = static_cast<float>(m_Health) / static_cast<float>(m_MaxHealth);
		pc->SetProgressBarPercent(percentage);

		if (percentage > 0.8f)
		{
			// Green
			pc->SetTexture(PROGRESS_BAR_TYPE::PROGRESS_BAR_TYPE_FRONT, m_100);
		}
		else if (percentage > 0.6f)
		{
			// Lime
			pc->SetTexture(PROGRESS_BAR_TYPE::PROGRESS_BAR_TYPE_FRONT, m_80);
		}
		else if (percentage > 0.4f)
		{
			// Yellow
			pc->SetTexture(PROGRESS_BAR_TYPE::PROGRESS_BAR_TYPE_FRONT, m_60);
		}
		else if (percentage > 0.2f)
		{
			// Orange
			pc->SetTexture(PROGRESS_BAR_TYPE::PROGRESS_BAR_TYPE_FRONT, m_40);
		}
		else if (percentage > 0.0f)
		{
			// Red
			pc->SetTexture(PROGRESS_BAR_TYPE::PROGRESS_BAR_TYPE_FRONT, m_20);
		}
	}
}

void component::HealthComponent::TakeDamage(int damage)
{
	// Call on upgrade on damage functions
	if (m_pParent->HasComponent<component::UpgradeComponent>())
	{
		m_pParent->GetComponent<component::UpgradeComponent>()->OnDamage();
	}
	damage = (damage - m_FlatDamageReduction) * m_MultiplicativeDamageReduction; //Flat Damage gets applied first followed by multaplicative damage
	if (damage <= 0)
	{
		damage = 1;
	}
	ChangeHealth(-damage);
	//Damage reduction stat is reset to allow upgrade to change again
	m_FlatDamageReduction = 0.0;
	m_MultiplicativeDamageReduction = 1.0;
}

void component::HealthComponent::ChangeFlatDamageReduction(int flatDamageReduction)
{
	m_FlatDamageReduction += flatDamageReduction;
}

void component::HealthComponent::ChangeMultiplicativeDamageReduction(float multaplicativeDamageReduction)
{
	m_MultiplicativeDamageReduction *= multaplicativeDamageReduction;
}

int component::HealthComponent::GetHealth() const
{
	return m_Health;
}

int component::HealthComponent::GetMaxHealth() const
{
	return m_MaxHealth;
}

void component::HealthComponent::SetMaxHealth(int newHealth)
{
	m_MaxHealth = newHealth;
}

void component::HealthComponent::ChangeMaxHealth(int hpChange)
{
	m_MaxHealth += hpChange;
	m_Health = m_MaxHealth;
}

void component::HealthComponent::Reset()
{
	if (m_pParent->GetName() == "player")
	{
		m_MaxHealth = Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->GetMaxHealth();
	}

	m_Health = m_MaxHealth;
	m_Dead = false;
}