#include "UpgradeComponent.h"
#include "Upgrades/Upgrade.h"
#include "stdafx.h"

component::UpgradeComponent::UpgradeComponent(Entity* parent)
	:Component(parent)
{
}

component::UpgradeComponent::~UpgradeComponent()
{
	for (auto u : m_AppliedUpgrades)
	{
		delete u.second;
	}
}

void component::UpgradeComponent::Update(double dt)
{
}

void component::UpgradeComponent::RenderUpdate(double dt)
{
}

void component::UpgradeComponent::OnInitScene()
{
}

void component::UpgradeComponent::OnUnInitScene()
{
}

void component::UpgradeComponent::AddUpgrade(Upgrade* upgrade)
{
	if (upgrade->GetName() == "")
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "You need to name your upgrade!\n");
	}
	if (upgrade->GetType() == 0)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "You need to set type/types for your upgrade!\n");
	}

	m_AppliedUpgrades.emplace(std::make_pair(upgrade->GetName(), upgrade));
	upgrade->ApplyBoughtUpgrade();
}

void component::UpgradeComponent::RemoveUpgrade(Upgrade* upgrade)
{
	std::map<std::string, Upgrade*>::iterator it = m_AppliedUpgrades.find(upgrade->GetName());
	if (it != m_AppliedUpgrades.end())
	{
		m_AppliedUpgrades.erase(it);
	}
}

void component::UpgradeComponent::RemoveAllUpgrades()
{
	m_AppliedUpgrades.clear();
}

bool component::UpgradeComponent::HasUpgrade(std::string name)
{
	std::map<std::string, Upgrade*>::iterator it = m_AppliedUpgrades.find(name);
	if (it != m_AppliedUpgrades.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::map<std::string, Upgrade*> component::UpgradeComponent::GetUpgradeMap()
{
	return m_AppliedUpgrades;
}

Upgrade* component::UpgradeComponent::GetUpgradeByName(std::string name)
{
	return m_AppliedUpgrades[name];
}

void component::UpgradeComponent::OnHit()
{
	for (auto& upgrade : m_AppliedUpgrades)
	{
		upgrade.second->OnHit();
	}
}

void component::UpgradeComponent::OnRangedHit()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->OnRangedHit();
	}
}

void component::UpgradeComponent::OnMeleeHit()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->OnMeleeHit();
	}
}

void component::UpgradeComponent::OnDamage()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->OnDamage();
	}
}

void component::UpgradeComponent::OnPickUp()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->OnPickUp();
	}
}

void component::UpgradeComponent::OnDeath()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->OnDeath();
	}
}

void component::UpgradeComponent::RangedFlight()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->RangedFlight();
	}
}

void component::UpgradeComponent::RangedModifier()
{
	for (auto upgrade : m_AppliedUpgrades)
	{
		upgrade.second->RangedModifier();
	}
}
