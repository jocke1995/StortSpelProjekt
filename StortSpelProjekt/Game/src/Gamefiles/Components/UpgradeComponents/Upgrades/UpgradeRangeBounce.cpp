#include "UpgradeRangeBounce.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/RangeComponent.h"

UpgradeRangeBounce::UpgradeRangeBounce(Entity* parentEntity) : Upgrade(parentEntity)
{
	SetName("UpgradeRangeBounce");
	SetType(F_UpgradeType::PLAYER);

	m_Price = 1000;
	m_StartingPrice = m_Price;
	m_Description = "Makes your projectiles bouncy.";
	m_MaxLevel = 1;

	m_ImageName = "Bounce.png";
}

UpgradeRangeBounce::~UpgradeRangeBounce()
{
}

void UpgradeRangeBounce::IncreaseLevel()
{
}

void UpgradeRangeBounce::ApplyBoughtUpgrade()
{
	m_pParentEntity->GetComponent<component::RangeComponent>()->MakeProjectileBouncy(true);
}
