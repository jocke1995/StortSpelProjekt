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
	m_Price = 200;
	m_StartingPrice = m_Price;

	m_ImageName = "BlueJewel.png";
	
	m_StartDamageReduction = 0.75; // 25% damage reduction
	m_DamageReduction = m_StartDamageReduction;
	m_HealthThreshold = 0.5; // 50%
}

UpgradeBlueJewel::~UpgradeBlueJewel()
{

}

void UpgradeBlueJewel::OnDamage()
{
	if (m_pParentEntity->GetComponent<component::HealthComponent>()->GetHealth() <= float(m_pParentEntity->GetComponent<component::HealthComponent>()->GetMaxHealth() * m_HealthThreshold))
	{
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeMultiplicativeDamageReduction(m_DamageReduction);
	}
}

void UpgradeBlueJewel::IncreaseLevel()
{
	m_Level++;
	// increase the boost
	m_DamageReduction = pow(m_StartDamageReduction, m_Level);
	// increase the price of the upgrade
	m_Price += m_StartingPrice;
}

std::string UpgradeBlueJewel::GetDescription(unsigned int level)
{
	return "Blue Jewel: An iridecent blue jewel that makes the wearer only take " + std::to_string(static_cast<int>(pow(m_StartDamageReduction, level) * 100)) + "\% of damage taken while under " + std::to_string(static_cast<int>(m_HealthThreshold*100)) + "\% max health";
}
