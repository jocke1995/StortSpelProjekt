#include "CurrencyComponent.h"

component::CurrencyComponent::CurrencyComponent(Entity* parent, int balance) : Component(parent)
{
	m_balance = balance;
}

component::CurrencyComponent::~CurrencyComponent()
{
}

void component::CurrencyComponent::SetBalance(int newBalance)
{
	m_balance = newBalance;
}

void component::CurrencyComponent::ChangeBalance(int change)
{
	m_balance += change;
}

int component::CurrencyComponent::GetBalace() const
{
	return m_balance;
}
