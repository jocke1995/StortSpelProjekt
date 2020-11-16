#include "UpgradePoisonAttack.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "Components/ProjectileComponent.h"
#include "Components/PoisonDebuff.h"
#include <sstream>

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
	m_NrOfTicks = 9;
	m_TickDuration = 0.5;
	m_Slow = 0.1;
}

UpgradePoisonAttack::~UpgradePoisonAttack()
{
}

void UpgradePoisonAttack::IncreaseLevel()
{
	m_Level++;
	m_NrOfTicks = 9 + m_Level;
	m_Damage = (float)(2 + m_Level) / 3.0f;
	m_Slow = 0.05 + (float)m_Level / 20.0f;
	m_Price += m_StartingPrice;

}

void UpgradePoisonAttack::OnRangedHit(Entity* target)
{
	if (target->HasComponent<component::HealthComponent>())
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
}

void UpgradePoisonAttack::ApplyBoughtUpgrade()
{
	// This upgrade only needs to increase price for next level when bought
	m_Price *= 2;
}

std::string UpgradePoisonAttack::GetDescription(unsigned int level)
{
	std::string str = "Poison Attack: Causes your projectile to apply a poison, that deals ";
	std::ostringstream damage;
	damage.precision(2);
	damage << std::fixed << ((float)(2 + level) / 3.0f) * (9 + level);
	str += damage.str();
	str += " damage over ";
	std::ostringstream duration;
	duration.precision(1);
	duration << std::fixed << (float)(9 + level) * m_TickDuration;
	str += duration.str();
	str += " seconds and slows the enemy by ";
	std::ostringstream slow;
	slow.precision(0);
	slow << std::fixed << (0.05f + (float)level / 20.0f) * 100;
	str += slow.str();
	return 	 str + " \%";

}
