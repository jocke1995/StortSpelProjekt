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
	m_BaseAttackSpeed = m_pParentEntity->GetComponent<component::RangeComponent>()->GetAttackInterval();
	m_AttackPerSecond = 1.0f / m_BaseAttackSpeed;
	// how many percent do you want to increase attack speed with?
	m_Percent = 10;
	m_UpgradeFactor = m_AttackPerSecond * (static_cast<float>(m_Percent)/100.0);
}

UpgradeRangeAttackSpeed::~UpgradeRangeAttackSpeed()
{
}

void UpgradeRangeAttackSpeed::ApplyStat()
{
	m_AttackPerSecond += m_UpgradeFactor;
	m_pParentEntity->GetComponent<component::RangeComponent>()->SetAttackInterval(1.0f / m_AttackPerSecond);
	Log::Print("RANGE: %f\n", 1.0f / m_AttackPerSecond);
}

void UpgradeRangeAttackSpeed::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeRangeAttackSpeed::IncreaseLevel()
{
	m_Level++;
	m_Price = m_StartingPrice * pow(2, m_Level);
}

std::string UpgradeRangeAttackSpeed::GetDescription(unsigned int level)
{
	return "Range Attack Speed: increases ranged attack speed with " + std::to_string(m_Percent * level) + "\% (" + std::to_string(m_Percent) + "\% per level)";
}
