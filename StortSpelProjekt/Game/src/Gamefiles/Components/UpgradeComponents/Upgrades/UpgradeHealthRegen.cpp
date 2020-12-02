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
	//This gives a health regeneration of 4hp per second per level or 1hp every 0.25 seconds per level
	m_HealthTimer += dt * (m_Level * 2);
	while (m_HealthTimer >= 1)
	{
		m_HealthTimer -= 1;
		m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(1);
	}
}

void UpgradeHealthRegen::IncreaseLevel()
{
	m_Level++;

	// increase the price of the upgrade
	m_Price = m_StartingPrice * (m_Level + 1);
}

std::string UpgradeHealthRegen::GetDescription(unsigned int level)
{
	return "Health Regeneration: Passively heals the player " + std::to_string(level * 2) + " health point every second";
}
