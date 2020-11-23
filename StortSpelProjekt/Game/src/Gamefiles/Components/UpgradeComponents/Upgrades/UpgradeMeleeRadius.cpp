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
	m_Price = 100;
	m_StartingPrice = m_Price;

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
	m_pParentEntity->GetComponent<component::MeleeComponent>()->ChangeMeleeRadius();
	Log::Print("TEST!\n");
}

void UpgradeMeleeRadius::IncreaseLevel()
{
	m_Level++;
}

std::string UpgradeMeleeRadius::GetDescription(unsigned int level)
{
	return "Hello these are some words\n";
}
