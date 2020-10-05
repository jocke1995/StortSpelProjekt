#include "UpgradeRangeComponent.h"
#include "stdafx.h"

component::UpgradeRangeComponent::UpgradeRangeComponent(Entity* parent)
	:UpgradeComponent(parent)
{
}

component::UpgradeRangeComponent::~UpgradeRangeComponent()
{
}

void component::UpgradeRangeComponent::RangedHit()
{
	Log::Print("UpgradeRangeComponent called\n");
}
