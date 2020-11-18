#ifndef UPGRADE_MELEE_ATTACK_RATE_H
#define UPGRADE_MELEE_ATTACK_RATE_H
#include "Upgrade.h"

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
	float m_upgradeFactor;

	float newInterval();

};

#endif
