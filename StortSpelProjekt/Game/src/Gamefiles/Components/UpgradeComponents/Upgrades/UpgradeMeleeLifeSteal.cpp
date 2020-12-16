#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "UpgradeMeleeLifeSteal.h"
#include "Components/MeleeComponent.h"

// Gives 10% lifesteal to melee attacks. Subsequent levels will add 5% more lifesteal. 
// This procentage is based on the melee damage you do so 50 meleeDamage will give 5hp in lifesteal.
// Price scaling should be 300 -> 500 -> 700 -> 900 -> 1100 -> 1300 -> 1500 -> 1700 -> 1900 -> 2100 -> 2300 -> 2500 -> 2700 -> 2900 -> 3100 -> 3300 -> 3500 etc.
// Lifesteal at these lvls 10% -> 15% -> 20% -> 25% -> 30%  -> 35%  -> 40%  -> 45%  -> 50%  -> 55%  -> 60%  -> 65%  -> 70%  -> 75%  -> 80%  -> 85%  -> 90%  -> 95%  -> 100%  etc.
UpgradeMeleeLifeSteal::UpgradeMeleeLifeSteal(Entity* parent) : Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeMeleeLifeSteal");
	// set the upgrade type/types!
	SetType(F_UpgradeType::PLAYER);
	// set the price of this upgrade
	m_Price = 300;
	m_StartingPrice = m_Price;
	// This is max level because at lvl 17 lifesteal is % of meleeDamage.
	m_MaxLevel = 17;

	// percentage of damage done to steal as life
	m_PercentageGain = 0.10;

	m_ImageName = "MeleeDamageHealthRegen.png";
}

UpgradeMeleeLifeSteal::~UpgradeMeleeLifeSteal()
{
}

void UpgradeMeleeLifeSteal::IncreaseLevel()
{
	m_Level++;
	m_PercentageGain = 0.10f + 0.05f * (m_Level - 1); // Starts at 10% and increases by 5% per level
	// m_Level starts at 0 so we increase it by 1.
	// Pricing:  300 -> 500 -> 700 -> 900 -> 1100 -> 1300 -> 1500 -> 1700 -> 1900 -> 2100 -> 2300 -> 2500 -> 2700 -> 2900 -> 3100 -> 3300 -> 3500
	// Lifesteal 10% -> 15% -> 20% -> 25% -> 30%  -> 35%  -> 40%  -> 45%  -> 50%  -> 55%  -> 60%  -> 65%  -> 70%  -> 75%  -> 80%  -> 85%  -> 90%  -> 95%  -> 100%
	m_Price = m_StartingPrice + (200 * m_Level); 
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
	return "Melee Lifesteal: Converts " + std::to_string(static_cast<int>((0.10f + 0.05f * (level - 1)) * 100)) + "\% of melee damage done into own health.";
}
