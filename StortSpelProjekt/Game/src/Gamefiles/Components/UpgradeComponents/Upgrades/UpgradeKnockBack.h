#ifndef UPGRADEKNOCKBACK_H
#define UPGRADEKNOCKBACK_H
#include "Upgrade.h"

// Upgrade description: This upgrade increases the knockback stat with 5 each level. Price increases with 100 per level.
class UpgradeKnockBack : public Upgrade
{
public:
	UpgradeKnockBack(Entity* parent);
	~UpgradeKnockBack();

	void ApplyStat();
	void ApplyBoughtUpgrade();
	void IncreaseLevel();

	std::string GetDescription(unsigned int level);

private:
	float m_KnockBackAdder;
};

#endif
