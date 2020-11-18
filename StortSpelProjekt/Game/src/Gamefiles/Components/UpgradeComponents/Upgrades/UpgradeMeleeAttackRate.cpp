#include "UpgradeMeleeAttackRate.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeAttackRate::UpgradeMeleeAttackRate(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeAttackRate");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "MeleeDamage.png";
	m_upgradeFactor = 0.9;
}

UpgradeMeleeAttackRate::~UpgradeMeleeAttackRate()
{
}

void UpgradeMeleeAttackRate::ApplyStat()
{
	float newCooldown = newInterval();
	Log::Print("new interval: %f\n", newCooldown);
	m_pParentEntity->GetComponent<component::MeleeComponent>()->SetAttackInterval(newCooldown);
	Log::Print("APPLYSTAT\n");
}

void UpgradeMeleeAttackRate::ApplyBoughtUpgrade()
{
	ApplyStat();
	Log::Print("APPLYBOUGHTUPGRADE\n");
}

void UpgradeMeleeAttackRate::IncreaseLevel()
{
	m_Level++;
	m_Price = m_StartingPrice * m_Level;
	Log::Print("INCREASELEVEL\n");
}

std::string UpgradeMeleeAttackRate::GetDescription(unsigned int level)
{
	if (level == m_Level)
	{
		return "Melee Attack Rate: Decreases the cooldown for melee attacks to " + std::to_string(m_pParentEntity->GetComponent<component::MeleeComponent>()->GetAttackInterval());
	}
	else
	{
		float newVal = newInterval();
		return "Melee Attack Rate: Decreases the cooldown for melee attacks to " + std::to_string(newVal);
	}
}

float UpgradeMeleeAttackRate::newInterval()
{
	float oldInterval = m_pParentEntity->GetComponent<component::MeleeComponent>()->GetAttackInterval();
	float newInterval = oldInterval * m_upgradeFactor;
	return newInterval;
}
