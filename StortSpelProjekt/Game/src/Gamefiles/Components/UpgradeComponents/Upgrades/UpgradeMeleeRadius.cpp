#include "UpgradeMeleeRadius.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradeMeleeRadius::UpgradeMeleeRadius(Entity* parent) : Upgrade(parent)
{
	// set the name of the upgrade
	SetName("UpgradeMeleeRadius");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 400;
	m_StartingPrice = m_Price;
	m_MaxLevel = 3;

	m_ImageName = "MeleeDamage.png";

	m_Xscale = 1.5f;
	m_Zscale = 1.5f;
}

UpgradeMeleeRadius::~UpgradeMeleeRadius()
{
}

void UpgradeMeleeRadius::ApplyBoughtUpgrade()
{
	ApplyStat();
}

void UpgradeMeleeRadius::ApplyStat()
{
	if (m_Level == 0)
	{
		m_Xscale = 1.75f;
		m_Zscale = 1.75f;
	}
	else if (m_Level == 1)
	{
		m_Xscale = (m_Level) + 1;
		m_Zscale = (m_Level) + 1;
	}
	else if (m_Level == 2)
	{
		m_Xscale = (m_Level) + 0.2f;
		m_Zscale = (m_Level) + 0.2f;
	}
	
	m_pParentEntity->GetComponent<component::MeleeComponent>()->ChangeMeleeRadius(m_Xscale, m_Zscale);
}

void UpgradeMeleeRadius::IncreaseLevel()
{
	m_Level++;
	m_Price = m_StartingPrice * (m_Level + 1);
}

std::string UpgradeMeleeRadius::GetDescription(unsigned int level)
{
	return "Melee Radius: Increases scale of base melee attack range both forward and to the sides with 50, 75 and 100\% at max level 3.";
}
