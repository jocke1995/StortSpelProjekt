#include "UpgradeComponent.h"
#include "stdafx.h"

component::UpgradeComponent::UpgradeComponent(Entity* parent, std::string name)
	:Component(parent)
{
	m_Name = name;
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
