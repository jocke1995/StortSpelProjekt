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
	m_Price = 500;
	m_StartingPrice = m_Price;
	m_MaxLevel = 3;

	m_ImageName = "MeleeDamage.png";

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
	float x = (m_Level + 1) * 1.0f;
	float z = (m_Level + 1) * 1.0f;
	m_pParentEntity->GetComponent<component::MeleeComponent>()->ChangeMeleeRadius(x, z);
	Log::Print("x: %f\n z: %f\n", x, z);
}

void UpgradeMeleeRadius::IncreaseLevel()
{
	m_Level++;
	m_Price = m_Price * (m_Level + 1);
}

std::string UpgradeMeleeRadius::GetDescription(unsigned int level)
{
	return "Hello these are some words\n";
}
