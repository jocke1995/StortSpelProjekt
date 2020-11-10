#include "UpgradePotionOfStrength.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

UpgradePotionOfStrength::UpgradePotionOfStrength(Entity* parent) : Upgrade(parent)
{
	// Set the name of the upgrade
	SetName("PotionOfStrength");

	// Set type
	SetType(F_UpgradeType::PLAYER);

	m_StartingPrice = 100;
	m_Price = m_StartingPrice;
	m_IsActive = false;

	m_Description = "Temporarily upgrades the attack damage of both ranged and melee. Boost is removed at the end of the level.";
	m_ImageName = "PotionOfStrength.png";
}

UpgradePotionOfStrength::~UpgradePotionOfStrength()
{
}

void UpgradePotionOfStrength::IncreaseLevel()
{
	m_Price = (++m_Level + 2) * 100;
	ApplyStat();
}

void UpgradePotionOfStrength::ApplyStat()
{
	component::RangeComponent* cmpRange = m_pParentEntity->GetComponent<component::RangeComponent>();
	component::MeleeComponent* cmpMelee = m_pParentEntity->GetComponent<component::MeleeComponent>();

	if (cmpRange)
	{
		m_RangeChange = cmpRange->GetDamage() * (1.1 + m_Level) - cmpRange->GetDamage();
		cmpRange->ChangeDamage(m_RangeChange);
	}
	if (cmpMelee)
	{
		m_MeleeChange = cmpMelee->GetDamage() * (1.1 + m_Level) - cmpMelee->GetDamage();
		cmpMelee->ChangeDamage(m_MeleeChange);
	}
	m_IsActive = true;
}

void UpgradePotionOfStrength::onLevelEnd(LevelDone* evnt)
{
	if (m_IsActive)
	{
		component::RangeComponent* cmpRange = m_pParentEntity->GetComponent<component::RangeComponent>();
		component::MeleeComponent* cmpMelee = m_pParentEntity->GetComponent<component::MeleeComponent>();

		if (cmpRange)
		{
			cmpRange->ChangeDamage(-m_RangeChange);
		}
		if (cmpMelee)
		{
			cmpMelee->ChangeDamage(-m_MeleeChange);
		}
		m_IsActive = false;
	}
}
