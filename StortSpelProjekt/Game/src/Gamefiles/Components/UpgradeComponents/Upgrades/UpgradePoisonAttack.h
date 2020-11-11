#ifndef UPGRADEPOISONATTACK_H
#define UPGRADEPOISONATTACK_H
#include "Upgrade.h"

class Entity;

// Upgrade description: This upgrade gives the player life steal based on ranged attack damage. First level gives 10 percent, next levels increase 5 percent per level.  Price doubles per level.
class UpgradePoisonAttack : public Upgrade
{
public:
	UpgradePoisonAttack(Entity* parent);
	~UpgradePoisonAttack();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnRangedHit(Entity* target);
	void ApplyBoughtUpgrade();

private:
	int m_Damage;
	int m_NrOfTicks;
	float m_Slow;
	double m_TickDuration;

};

#endif
