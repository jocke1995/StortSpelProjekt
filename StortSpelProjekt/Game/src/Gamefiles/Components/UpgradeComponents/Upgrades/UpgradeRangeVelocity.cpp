#include "UpgradeRangeVelocity.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/RangeComponent.h"

UpgradeRangeVelocity::UpgradeRangeVelocity(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeVelocity");
	// set upgrade type/types!
	SetType(F_UpgradeType::PLAYER);		//The range velocity is set on a rangecomponent which gones on the player, not the projectile entity
	// set the price of this upgrade 
	m_Price = 1;
	m_StartingPrice = m_Price;
	// set short description 
	m_Description = "Range Velocity: Increases velocity of the range attacks with 10 per level. Max velocity is 100, at upgrade level 5";
	// this upgrade will have a max level
	m_MaxLevel = 5;
}

UpgradeRangeVelocity::~UpgradeRangeVelocity()
{
}

void UpgradeRangeVelocity::IncreaseLevel()
{
	if (m_Level < m_MaxLevel)
	{
		m_Level++;
		float oldVelocity = m_pParentEntity->GetComponent<component::RangeComponent>()->GetVelocity();
		// double the velocity of the shots
		float newVelocity = oldVelocity + 10;
		m_pParentEntity->GetComponent<component::RangeComponent>()->SetVelocity(newVelocity);
		m_Price = m_Price * 2;
	}
}

void UpgradeRangeVelocity::ApplyBoughtUpgrade()
{
	// get current velocity
	float currentVelocity = m_pParentEntity->GetComponent<component::RangeComponent>()->GetVelocity();
	// double the velocity of the shots
	float newVelocity = currentVelocity + 10;
	m_pParentEntity->GetComponent<component::RangeComponent>()->SetVelocity(newVelocity);

	// increase price for next level
	m_Price *= 2;
}
