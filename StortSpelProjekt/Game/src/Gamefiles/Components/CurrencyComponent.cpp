#include "CurrencyComponent.h"
#include "../Headers/EngineMath.h"
#include "../ECS/Entity.h"
#include "../Events/EventBus.h"

component::CurrencyComponent::CurrencyComponent(Entity* parent, int balance) : Component(parent)
{
	m_Balance = balance;
	m_ComponentActive = true;
	EventBus::GetInstance().Subscribe(this, &CurrencyComponent::ChangeBalance);
}

component::CurrencyComponent::~CurrencyComponent()
{
}

void component::CurrencyComponent::OnInitScene()
{
}

void component::CurrencyComponent::OnUnInitScene()
{
}

void component::CurrencyComponent::SetBalance(int newBalance)
{
	m_Balance = newBalance;
}

void component::CurrencyComponent::ChangeBalance(Death* evnt)
{
	if (m_ComponentActive)
	{
		std::string enemyType = evnt->ent->GetName();
		if (enemyType.find("enemyZombie") != std::string::npos)
		{
			m_Balance += 20;
		}
		else if (enemyType.find("enemyConan") != std::string::npos)
		{
			m_Balance += 30;
		}
		else if (enemyType.find("enemyDemon") != std::string::npos)
		{
			m_Balance += 30;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Unknown enemy type, defaults to balance of 10 SEK \n");
			m_Balance += 10;
		}
	}
	else 
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Death event called without currency component active on player, enemy currently not giving the player any curreny");
	}
}

void component::CurrencyComponent::ChangeBalance(int change)
{
	m_Balance += change;
}

int component::CurrencyComponent::GetBalace() const
{
	return m_Balance;
}
