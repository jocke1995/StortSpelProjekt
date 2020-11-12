#include "UpgradeBlueJewel.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeBlueJewel::UpgradeBlueJewel(Entity* parentEntity) : Upgrade(parentEntity)
{
	// set the name of the upgrade
	SetName("UpgradeBlueJewel");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 1000;
	m_StartingPrice = m_Price;

	m_ImageName = "BlueJewel.png";
	
	m_StartDamageReduction = 0.4; // 40%
	m_DamageReduction = m_StartDamageReduction;
	m_HealthThreshold = 0.3; // 30%
}

UpgradeBlueJewel::~UpgradeBlueJewel()
{

}

void UpgradeBlueJewel::OnDamage()
{
	if (m_pParentEntity->GetComponent<component::HealthComponent>()->GetHealth() <= float(m_pParentEntity->GetComponent<component::HealthComponent>()->GetMaxHealth() * 0.3))
	{
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeProcentileDamageReduction(m_DamageReduction);
	}
}

void UpgradeBlueJewel::IncreaseLevel()
{
	m_Level++;
	// increase the boost
	m_DamageReduction = pow(m_StartDamageReduction, m_Level);
	// increase the price of the upgrade
	m_Price = m_StartingPrice * m_Level;
}

std::string UpgradeBlueJewel::GetDescription(unsigned int level)
{
	return "Blue Jewel: An iridecent blue jewel that reduces damage taken by " + std::to_string(pow(m_StartDamageReduction, level) * 100) + "\% while under " + std::to_string(m_HealthThreshold) + "\% max health";
}
