#ifndef UPGRADEMELEELIFESTEAL_H
#define UPGRADEMELEELIFESTEAL_H

#include "Upgrade.h"


class Entity;
class UpgradeMeleeLifeSteal : public Upgrade
{
public:
	UpgradeMeleeLifeSteal(Entity* parent);
	~UpgradeMeleeLifeSteal();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnMeleeHit(Entity* target);
	void ApplyBoughtUpgrade();

	std::string GetDescription(unsigned int level);

private:
	float m_PercentageGain;

};

#endif // !UPGRADEMELEELIFESTEAL_H