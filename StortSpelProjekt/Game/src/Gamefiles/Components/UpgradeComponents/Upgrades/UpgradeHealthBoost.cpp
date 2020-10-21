#include "UpgradeHealthBoost.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeHealthBoost::UpgradeHealthBoost(Entity* parentEntity) : Upgrade(parentEntity)
{
	// set the name of the upgrade
	SetName("UpgradeHealthBoost");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 5;
	m_StartingPrice = m_Price;
	
	m_HealthBoost = 10;
	
	// Write a description for the upgrade
	m_Description = "Gives player a health increase. At level 1 the increase is 10 hp. The following levels it will be ((10 / 2) * level). So level 5 will be 25";

}

UpgradeHealthBoost::~UpgradeHealthBoost()
{

}

void UpgradeHealthBoost::ApplyStat()
{
	m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(m_HealthBoost);
}

void UpgradeHealthBoost::IncreaseLevel()
{
	m_Level++;
	// increase the boost
	m_HealthBoost = (m_HealthBoost / 2) * m_Level;
	// increase the price of the upgrade
	m_Price = m_StartingPrice * m_Level;
	ApplyStat();
}
