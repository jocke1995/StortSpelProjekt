#include "UpgradeRangeLifeSteal.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "UpgradeMeleeLifeSteal.h"
#include "Components/MeleeComponent.h"

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
	m_Price = 300 * pow(m_Level + 1, 2);
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
	return "Melee Lifesteal: Converts a " + std::to_string(static_cast<int>((0.05f + 0.05f * level) * 100)) + "\% of melee damage done into own health.";
}
