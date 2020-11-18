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
	m_BaseAttackSpeed = m_pParentEntity->GetComponent<component::MeleeComponent>()->GetAttackInterval();
	m_AttackPerSecond = 1.0f / m_BaseAttackSpeed;
	m_UpgradeFactor = m_AttackPerSecond * 0.2f;
}

UpgradeMeleeAttackRate::~UpgradeMeleeAttackRate()
{
}

void UpgradeMeleeAttackRate::ApplyStat()
{
	m_AttackPerSecond = m_AttackPerSecond + m_UpgradeFactor;
	m_pParentEntity->GetComponent<component::MeleeComponent>()->SetAttackInterval(1.0f / m_AttackPerSecond);
}

void UpgradeMeleeAttackRate::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeMeleeAttackRate::IncreaseLevel()
{
	m_Level++;
	m_Price *= 2;
}

std::string UpgradeMeleeAttackRate::GetDescription(unsigned int level)
{
	return "Melee Attack Speed: increases attack speed with " + std::to_string(20 * level) + "\% (20\% per level)";
}
