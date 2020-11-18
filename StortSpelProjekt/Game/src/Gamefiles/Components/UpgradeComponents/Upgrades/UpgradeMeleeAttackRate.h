#ifndef UPGRADE_MELEE_ATTACK_RATE_H
#define UPGRADE_MELEE_ATTACK_RATE_H
#include "Upgrade.h"

// Upgrade description: This upgrade increases melee attack speed with 20% of base attack speed per level . Price doubles per level.
class UpgradeMeleeAttackRate : public Upgrade
{
public:
	UpgradeMeleeAttackRate(Entity* parent);
	~UpgradeMeleeAttackRate();

	void ApplyStat();
	void ApplyBoughtUpgrade();
	void IncreaseLevel();

	std::string GetDescription(unsigned int level);

private:
	float m_UpgradeFactor;
	float m_BaseAttackSpeed;
	float m_AttackPerSecond;
};

#endif
