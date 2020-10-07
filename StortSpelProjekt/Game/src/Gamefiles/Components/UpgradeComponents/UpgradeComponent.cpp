#include "UpgradeComponent.h"
#include "stdafx.h"

component::UpgradeComponent::UpgradeComponent(Entity* parent)
	:Component(parent)
{
}

component::UpgradeComponent::~UpgradeComponent()
{
}

void component::UpgradeComponent::Update(double dt)
{
}

void component::UpgradeComponent::RenderUpdate(double dt)
{
}

void component::UpgradeComponent::SetName(std::string name)
{
	m_Name = name;
}

std::string component::UpgradeComponent::GetName()
{
	return m_Name;
}

void component::UpgradeComponent::IncreaseLevel()
{
	m_Level++;
}

void component::UpgradeComponent::DecreaseLevel()
{
	m_Level--;
}

int component::UpgradeComponent::GetLevel()
{
	return m_Level;
}

void component::UpgradeComponent::SetType(int type)
{
	m_Type = type;
}

int component::UpgradeComponent::GetType()
{
	return m_Type;
}

void component::UpgradeComponent::OnHit()
{
}

void component::UpgradeComponent::RangedHit()
{
}

void component::UpgradeComponent::MeleeHit()
{
}

void component::UpgradeComponent::OnDamage()
{
}

void component::UpgradeComponent::OnPickUp()
{
}

void component::UpgradeComponent::OnDeath()
{
}

void component::UpgradeComponent::RangedFlight()
{
}

void component::UpgradeComponent::RangedModifier()
{
}
