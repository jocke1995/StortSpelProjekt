#include "UpgradeKnockBack.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeKnockBack::UpgradeKnockBack(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeKnockBack");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	m_ImageName = "MeleeDamage.png";
	m_KnockBackAdder = 10;
}

UpgradeKnockBack::~UpgradeKnockBack()
{
}

void UpgradeKnockBack::ApplyStat()
{
	m_pParentEntity->GetComponent<component::MeleeComponent>()->ChangeKnockBack(m_KnockBackAdder);
}

void UpgradeKnockBack::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeKnockBack::IncreaseLevel()
{
	m_Level++;
	m_Price = m_StartingPrice * (m_Level + 1);
}

std::string UpgradeKnockBack::GetDescription(unsigned int level)
{
	return "Knockback: Increases knockback stat with " + std::to_string(static_cast<int>(m_KnockBackAdder*level));
}
