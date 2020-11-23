#include "UpgradeMeleeAttackSpeed.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeAttackSpeed::UpgradeMeleeAttackSpeed(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeAtkSpeed");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "MeleeDamage.png";
	if (m_pParentEntity->HasComponent<component::MeleeComponent>())
	{
		m_BaseAttackSpeed = m_pParentEntity->GetComponent<component::MeleeComponent>()->GetAttackInterval();
		m_AttackPerSecond = 1.0f / m_BaseAttackSpeed;
	}
	else
	{
		m_BaseAttackSpeed = 1.0f;
		m_AttackPerSecond = 1.0f;
	}

	// how many percent do you want to increase attack speed with?
	m_Percent = 20;
	m_UpgradeFactor = m_AttackPerSecond * (static_cast<float>(m_Percent) / 100.0);
}

UpgradeMeleeAttackSpeed::~UpgradeMeleeAttackSpeed()
{
}

void UpgradeMeleeAttackSpeed::ApplyStat()
{
	m_AttackPerSecond = (1.0f / m_BaseAttackSpeed) + (m_UpgradeFactor * (m_Level + 1));
	m_pParentEntity->GetComponent<component::MeleeComponent>()->SetAttackInterval(1.0f / m_AttackPerSecond);
}

void UpgradeMeleeAttackSpeed::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeMeleeAttackSpeed::IncreaseLevel()
{
	m_Level++;
	m_Price = m_StartingPrice * pow(2, m_Level);
}

std::string UpgradeMeleeAttackSpeed::GetDescription(unsigned int level)
{
	return "Melee Attack Speed: increases melee attack speed with " + std::to_string(m_Percent * level) + "\% (" + std::to_string(m_Percent) + "\% per level)";
}
