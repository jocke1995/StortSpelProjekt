#ifndef UPGRADE_RANGE_ATTACK_SPEED
#define UPGRADE_RANGE_ATTACK_SPEED
#include "Upgrade.h"

// Upgrade description_ This upgrade increases range attack speed with 10% of base attack speed per level. Price doubles per level.
class UpgradeRangeAttackSpeed : public Upgrade
{
public:
	UpgradeRangeAttackSpeed(Entity* parent);
	~UpgradeRangeAttackSpeed();

	void ApplyStat();
	void ApplyBoughtUpgrade();
	void IncreaseLevel();

	std::string GetDescription(unsigned int level);

private:
	float m_UpgradeFactor;
	float m_BaseAttackSpeed;
	float m_AttackPerSecond;
	int m_Percent;
};

#endif
