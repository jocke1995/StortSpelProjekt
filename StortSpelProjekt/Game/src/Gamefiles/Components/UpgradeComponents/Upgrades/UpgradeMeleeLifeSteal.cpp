#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "UpgradeMeleeLifeSteal.h"
#include "Components/MeleeComponent.h"

// Gives 10% lifesteal to melee attacks. Subsequent levels will add 5% more lifesteal. 
// This procentage is based on the melee damage you do so 50 meleeDamage will give 5hp in lifesteal.
// Price scaling should be 300 -> 700 -> 1300 -> 2100 -> 3100 -> 4300 -> 5700 -> 7300 -> 9100 etc.
// Lifesteal at these lvls 10% -> 15% -> 20%  -> 25%  -> 30%  -> 35%  -> 40%  -> 45%  -> 50%  etc.
UpgradeMeleeLifeSteal::UpgradeMeleeLifeSteal(Entity* parent) : Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeMeleeLifeSteal");
	// set the upgrade type/types!
	SetType(F_UpgradeType::PLAYER);
	// set the price of this upgrade
	m_Price = 300;
	m_StartingPrice = m_Price;

	// percentage of damage done to steal as life
	m_PercentageGain = 0.10;

	m_ImageName = "MeleeDamage.png";
}

UpgradeMeleeLifeSteal::~UpgradeMeleeLifeSteal()
{
}

void UpgradeMeleeLifeSteal::IncreaseLevel()
{
	m_Level++;
	m_PercentageGain = 0.05f + 0.05f * m_Level; // Starts at 10% and increases by 5% per level
	m_Price += (m_Level + 1) * 200; // m_Level starts at 0 so we increase it by 1.
}

void UpgradeMeleeLifeSteal::OnMeleeHit(Entity* target)
{
	//Only Lifesteal of enemies
	if (strcmp(target->GetName().substr(0, 5).c_str(), "enemy") == 0)
	{
		// get damage from projectile
		int damage = m_pParentEntity->GetComponent<component::MeleeComponent>()->GetDamage();
		// calculate amount of health to add
		int hpChange = static_cast<int>(damage * m_PercentageGain);
		// add a bit of health
		Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->ChangeHealth(hpChange);
	}
}

void UpgradeMeleeLifeSteal::ApplyBoughtUpgrade()
{
}

std::string UpgradeMeleeLifeSteal::GetDescription(unsigned int level)
{
	return "Melee Lifesteal: Converts " + std::to_string(static_cast<int>((0.05f + 0.05f * level) * 100)) + "\% of melee damage done into own health.";
}
