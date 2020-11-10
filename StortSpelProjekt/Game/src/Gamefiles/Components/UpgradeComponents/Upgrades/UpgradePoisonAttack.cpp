#include "UpgradePoisonAttack.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "Components/ProjectileComponent.h"

UpgradePoisonAttack::UpgradePoisonAttack(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradePoisonAttack");
	// set the upgrade type/types!
	SetType(F_UpgradeType::RANGE);	// goes on the projectiles
	// set the price of this upgrade
	m_Price = 1;
	m_StartingPrice = m_Price;
	// set short description 
	m_Description = "Poison Attack: Causes your projectile to apply a poison, that deals 10 damage over 5 seconds and slows the enemy by 10%";
	// percentage of damage done to steal as life
	m_Damage = 1;
	m_NrOfTicks = 10;
	m_TickDuration = 0.5;
}

UpgradePoisonAttack::~UpgradePoisonAttack()
{
}

void UpgradePoisonAttack::IncreaseLevel()
{
	m_Level++;
	m_PercentageGain += 0.05;
	m_Price *= 2;
}

void UpgradePoisonAttack::OnRangedHit()
{
	// get damage from projectile
	int damage = m_pParentEntity->GetComponent<component::ProjectileComponent>()->GetDamage();
	// calculate amount of health to add
	int hpChange = static_cast<int>(damage * m_PercentageGain);
	// add a bit of health
	Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->ChangeHealth(hpChange);
}

void UpgradePoisonAttack::ApplyBoughtUpgrade()
{
	// This upgrade only needs to increase price for next level when bought
	m_Price *= 2;
}
