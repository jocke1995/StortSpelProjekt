#include "CurrencyComponent.h"

component::CurrencyComponent::CurrencyComponent(Entity* parent, int balance) : Component(parent)
{
	m_Balance = balance;
}

component::CurrencyComponent::~CurrencyComponent()
{
}

void component::CurrencyComponent::OnInitScene()
{
}

void component::CurrencyComponent::OnLoadScene()
{
}

void component::CurrencyComponent::OnUnloadScene()
{
}

void component::CurrencyComponent::SetBalance(int newBalance)
{
	m_Balance = newBalance;
}

void component::CurrencyComponent::ChangeBalance(int change)
{
	m_Balance += change;
}

int component::CurrencyComponent::GetBalace() const
{
	return m_Balance;
}
