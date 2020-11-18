#include "UpgradeHealthRegen.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include <sstream>
#include <iomanip>

UpgradeHealthRegen::UpgradeHealthRegen(Entity* parentEntity) : Upgrade(parentEntity)
{
	// set the name of the upgrade
	SetName("UpgradeHealthRegen");
	// set the type of the upgrade
	SetType(F_UpgradeType::PLAYER);
	// set the price of the upgrade
	m_Price = 100;
	m_StartingPrice = m_Price;

	//Variable to count to cou
	m_HealthTimer = 0;

	m_ImageName = "HealthRegen.png";
}

UpgradeHealthRegen::~UpgradeHealthRegen()
{

}

void UpgradeHealthRegen::Update(double dt)
{
	//count up the timer until we reach 1 to give one health.
	//This gives a health regeneration of 0.33hp per second per level or 1hp per 3 seconds per level
	m_HealthTimer += dt * (m_Level / 3.0f);
	if (m_HealthTimer >= 1)
	{
		m_HealthTimer -= 1;
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(1);
	}
}

void UpgradeHealthRegen::IncreaseLevel()
{
	m_Level++;

	// increase the price of the upgrade
	m_Price += m_StartingPrice;
}

std::string UpgradeHealthRegen::GetDescription(unsigned int level)
{
	std::ostringstream str;
	str << std::fixed << std::setprecision(2) << (3.0f / level);
	return "Health Regenration: Passively heals the player 1 health point every " + str.str();
}
