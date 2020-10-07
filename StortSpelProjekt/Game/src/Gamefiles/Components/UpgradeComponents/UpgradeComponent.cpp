#include "UpgradeComponent.h"
#include "stdafx.h"

component::UpgradeComponent::UpgradeComponent(Entity* parent, std::string name, int type)
	:Component(parent)
{
	m_Name = name;
	m_Type = type;
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

void component::UpgradeComponent::OnHit()
{
}

