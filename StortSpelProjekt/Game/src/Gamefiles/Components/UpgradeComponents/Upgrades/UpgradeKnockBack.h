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

	std::string GetDescription(unsigned int level);

private:
	float m_KnockBackAdder;
};

#endif
