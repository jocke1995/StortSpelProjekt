#include "UpgradeRangeComponent.h"
#include "stdafx.h"

component::UpgradeRangeComponent::UpgradeRangeComponent(Entity* parent, std::string name)
	:UpgradeComponent(parent, name)
{
}

component::UpgradeRangeComponent::~UpgradeRangeComponent()
{
}

void component::UpgradeRangeComponent::OnHit()
{
}

//void component::UpgradeRangeComponent::OnHit()
//{
//	Log::Print("UpgradeRangeComponent called\n");
//}

void component::UpgradeRangeComponent::RangedFlight()
{
}
