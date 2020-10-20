#include "UpgradeRangeVelocity.h"
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

}

UpgradeRangeVelocity::~UpgradeRangeVelocity()
{
}

void UpgradeRangeVelocity::IncreaseLevel()
{
	m_Level++;
	float oldVelocity = m_pParentEntity->GetComponent<component::RangeComponent>()->GetVelocity();
	// double the velocity of the shots
	float newVelocity = oldVelocity * 2;
	m_pParentEntity->GetComponent<component::RangeComponent>()->SetVelocity(newVelocity);
}

void UpgradeRangeVelocity::RangedFlight()
{
	//float oldVelocity = m_pParentEntity->GetComponent<component::RangeComponent>()->GetVelocity();
	//// double the velocity of the shots
	//float newVelocity = oldVelocity * 2;
	//m_pParentEntity->GetComponent<component::RangeComponent>()->SetVelocity(newVelocity);
}

void UpgradeRangeVelocity::ApplyStat()
{
}

void UpgradeRangeVelocity::ApplyBoughtUpgrade()
{
	float oldVelocity = m_pParentEntity->GetComponent<component::RangeComponent>()->GetVelocity();
	// double the velocity of the shots
	float newVelocity = oldVelocity * 2;
	m_pParentEntity->GetComponent<component::RangeComponent>()->SetVelocity(newVelocity);
}

