#include "UpgradePoisonAttack.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "Components/ProjectileComponent.h"
#include "Components/PoisonDebuff.h"

UpgradePoisonAttack::UpgradePoisonAttack(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradePoisonAttack");
	// set the upgrade type/types!
	SetType(F_UpgradeType::RANGE);	// goes on the projectiles
	// set the price of this upgrade
	m_Price = 120;
	m_StartingPrice = m_Price;

	m_ImageName = "PoisonAttack.png";

	// percentage of damage done to steal as life
	m_Damage = 1;
	m_NrOfTicks = 10;
	m_TickDuration = 0.5;
	m_Slow = 0.1;
	m_Description = "Poison Attack: Causes your projectile to apply a poison, that deals " + std::to_string(m_Damage * m_NrOfTicks) + " damage over " + std::to_string(m_NrOfTicks * m_TickDuration) + " seconds and slows the enemy by " + std::to_string(static_cast<int>(m_Slow * 100)) + " \%";
}

UpgradePoisonAttack::~UpgradePoisonAttack()
{
}

void UpgradePoisonAttack::IncreaseLevel()
{
	m_Level++;
	m_NrOfTicks = 10 + m_Level;
	m_Damage = (float)(1 + m_Level) / 3.0f;
	m_Slow = 0.05 + (float)m_Level / 50.0f;
	m_Price += m_StartingPrice;

	m_Description = "Poison Attack: Causes your projectile to apply a poison, that deals" + std::to_string(m_Damage * m_NrOfTicks) + " damage over " + std::to_string(m_NrOfTicks * m_TickDuration) + " seconds and slows the enemy by " + std::to_string(m_Slow * 100) + " \%";
}

void UpgradePoisonAttack::OnRangedHit(Entity* target)
{
	if (target->HasComponent<component::PoisonDebuff>())
	{
		target->GetComponent<component::PoisonDebuff>()->Reset(m_Damage, m_NrOfTicks, m_TickDuration, m_Slow);
	}
	else
	{
		target->AddComponent<component::PoisonDebuff>(m_Damage, m_NrOfTicks, m_TickDuration, m_Slow);
	}
}

void UpgradePoisonAttack::ApplyBoughtUpgrade()
{
	// This upgrade only needs to increase price for next level when bought
	m_Price *= 2;
}
