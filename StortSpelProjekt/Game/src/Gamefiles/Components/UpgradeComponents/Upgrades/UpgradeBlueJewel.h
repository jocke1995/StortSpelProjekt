#ifndef UPGRADE_BLUE_JEWEL_H
#define UPGRADE_BLUE_JEWEL_H

#include "Components/UpgradeComponents/Upgrades/Upgrade.h"

class UpgradeBlueJewel : public Upgrade
{
public:
	UpgradeBlueJewel(Entity* parentEntity);
	~UpgradeBlueJewel();

	void OnDamage();
	void IncreaseLevel();

private:
	float m_DamageReduction;

};

#endif // !UPGRADE_BLUE_JEWEL_H