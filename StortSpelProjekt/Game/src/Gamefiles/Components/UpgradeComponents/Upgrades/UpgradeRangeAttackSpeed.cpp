#include "UpgradeRangeAttackSpeed.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeRangeAttackSpeed::UpgradeRangeAttackSpeed(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeRangeAtkSpeed");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);	// changes rangecomponent, not the projectiles
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "RangeUpgrade.png";
	if (m_pParentEntity->HasComponent<component::MeleeComponent>())
	{
		m_BaseAttackSpeed = m_pParentEntity->GetComponent<component::RangeComponent>()->GetAttackInterval();
		m_AttackPerSecond = 1.0f / m_BaseAttackSpeed;
	}
	else
	{
		m_BaseAttackSpeed = 1.0f;
		m_AttackPerSecond = 1.0f;
	}

	// how many percent do you want to increase attack speed with?
	m_Percent = 10;
	m_UpgradeFactor = m_AttackPerSecond * (static_cast<float>(m_Percent)/100.0);
}

UpgradeRangeAttackSpeed::~UpgradeRangeAttackSpeed()
{
}

void UpgradeRangeAttackSpeed::ApplyStat()
{
	m_AttackPerSecond = (1.0f / m_BaseAttackSpeed) + (m_UpgradeFactor * (m_Level + 1));
	m_pParentEntity->GetComponent<component::RangeComponent>()->SetAttackInterval(1.0f / m_AttackPerSecond);
}

void UpgradeRangeAttackSpeed::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeRangeAttackSpeed::IncreaseLevel()
{
	m_Level++;
	// The pricing should work as 100 -> 150 -> 300 -> 450 -> 600 -> 750.
	// So lvl 5 would cost 750 and give 50% boost to attack speed.
	m_Price = 150 * m_Level;
}

std::string UpgradeRangeAttackSpeed::GetDescription(unsigned int level)
{
	return "Range Attack Speed: increases ranged attack speed with " + std::to_string(m_Percent * level) + "\% (" + std::to_string(m_Percent) + "\% per level)";
}
