#ifndef UPGRADEKNOCKBACK_H
#define UPGRADEKNOCKBACK_H
#include "Upgrade.h"

class UpgradeKnockBack : public Upgrade
{
public:
	UpgradeKnockBack(Entity* parent);
	~UpgradeKnockBack();

	void ApplyStat();
	void ApplyBoughtUpgrade();
	void IncreaseLevel();

private:
	float m_KnockBack;
};

#endif
