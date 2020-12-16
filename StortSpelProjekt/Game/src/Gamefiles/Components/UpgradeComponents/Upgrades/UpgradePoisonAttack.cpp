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
	m_MaxLevel = 8;

	m_ImageName = "PoisonAttack.png";

	m_Damage = 0.04;
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
	m_Damage = 0.04 + (float)(0.005 * (m_Level - 1));
	// Slow of 50% at maxlvlso enemies aren't snails.
	m_Slow = 0.10 + (0.05 * (m_Level - 1));
	m_Price = m_StartingPrice * (m_Level + 1);

}

void UpgradePoisonAttack::OnRangedHit(Entity* target, Entity* projectile)
{
	if (target->HasComponent<component::HealthComponent>())
	{
		if (target->HasComponent<component::PoisonDebuff>())
		{
			target->GetComponent<component::PoisonDebuff>()->Reset(m_pParentEntity->GetComponent<component::ProjectileComponent>()->GetDamage() * m_Damage, m_NrOfTicks, m_TickDuration, m_Slow);
		}
		else
		{
			target->AddComponent<component::PoisonDebuff>(m_pParentEntity->GetComponent<component::ProjectileComponent>()->GetDamage() * m_Damage, m_NrOfTicks, m_TickDuration, m_Slow);
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
	std::string str = "Poison Attack: Causes your projectile to apply a poison. Deals ";
	std::ostringstream damage;
	damage.precision(1);
	damage << std::fixed << (((0.04 + 0.005 * (level - 1) )* (9 + level))*100);
	str += damage.str();
	str += "% of range damage over ";
	std::ostringstream duration;
	duration.precision(1);
	duration << std::fixed << (float)(9 + level) * m_TickDuration;
	str += duration.str();
	str += " seconds and slows the enemy by ";
	std::ostringstream slow;
	slow.precision(0);
	slow << std::fixed << ((0.10 + ((float)level * 0.05))*100);
	str += slow.str();
	str += "\%";
	return str + ". Max level is 8.";

}
