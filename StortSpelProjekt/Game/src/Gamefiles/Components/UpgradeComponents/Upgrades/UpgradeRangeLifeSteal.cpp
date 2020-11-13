#include "UpgradeRangeLifeSteal.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "Components/ProjectileComponent.h"

UpgradeRangeLifeSteal::UpgradeRangeLifeSteal(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeRangeLifeSteal");
	// set the upgrade type/types!
	SetType(F_UpgradeType::RANGE);	// goes on the projectiles
	// set the price of this upgrade
	m_Price = 300;
	m_StartingPrice = m_Price;
	
	// percentage of damage done to steal as life
	m_PercentageGain = 0.10;
}

UpgradeRangeLifeSteal::~UpgradeRangeLifeSteal()
{
}

void UpgradeRangeLifeSteal::IncreaseLevel()
{
	m_Level++;
	m_PercentageGain = 0.05f + 0.05f * m_Level; // Starts at 10% and increases by 5% per level
	m_Price = 300 * pow(m_Level + 1,2);
}

void UpgradeRangeLifeSteal::OnRangedHit()
{
	// get damage from projectile
	int damage = m_pParentEntity->GetComponent<component::ProjectileComponent>()->GetDamage();
	// calculate amount of health to add
	int hpChange = static_cast<int>(damage * m_PercentageGain);
	// add a bit of health
	Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->ChangeHealth(hpChange);
}

void UpgradeRangeLifeSteal::ApplyBoughtUpgrade()
{
}

std::string UpgradeRangeLifeSteal::GetDescription(unsigned int level)
{
	return "Ranged Lifesteal: Converts a " + std::to_string(static_cast<int>((0.05f + 0.05f * level)*100)) + "\% of range damage done into own health.";
}
