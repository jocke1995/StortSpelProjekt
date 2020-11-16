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

	std::string GetDescription(unsigned int level);

private:
	float m_DamageReduction;
	float m_StartDamageReduction;
	float m_HealthThreshold;

};

#endif // !UPGRADE_BLUE_JEWEL_H