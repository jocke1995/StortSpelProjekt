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
	m_Price = 1;
	m_StartingPrice = m_Price;
	// set short description 
	m_Description = "Converts a percentage of range damage done into own health. 10\% for the first level, 5\% more per upgrade level";
	// percentage of damage done to steal as life
	m_PercentageGain = 0.10;
}

UpgradeRangeLifeSteal::~UpgradeRangeLifeSteal()
{
}

void UpgradeRangeLifeSteal::IncreaseLevel()
{
	m_Level++;
	m_PercentageGain += 0.05;
	m_Price *= 2;
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
	// This upgrade only needs to increase price for next level when bought
	m_Price *= 2;
}
