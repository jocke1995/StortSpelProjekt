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
	m_Price = 100;
	m_StartingPrice = m_Price;
	
	m_HealthBoost = 10;
	
	// Write a description for the upgrade
	m_Description = "Health Boost: Increases the players health by ten";

	m_ImageName = "HealthBoost.png";
}

UpgradeHealthBoost::~UpgradeHealthBoost()
{

}

void UpgradeHealthBoost::ApplyStat()
{
	m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeMaxHealth(m_HealthBoost);
}

void UpgradeHealthBoost::IncreaseLevel()
{
	m_Level++;
	// increase the price of the upgrade
	m_Price = m_StartingPrice + m_StartingPrice * ((float)m_Level/2.0f);
}

void UpgradeHealthBoost::ApplyBoughtUpgrade()
{
	ApplyStat();
}
