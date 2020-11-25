#ifndef UPGRADE_EXPLOSIVE_ATTACK_H
#define UPGRADE_EXPLOSIVE_ATTACK_H
#include "Upgrade.h"

class Entity;

static unsigned int m_ParticleCounter;

// Upgrade description: This upgrade gives the player life steal based on ranged attack damage. First level gives 10 percent, next levels increase 5 percent per level.  Price doubles per level.
class UpgradeExplosiveAttack : public Upgrade
{
public:
	UpgradeExplosiveAttack(Entity* parent);
	~UpgradeExplosiveAttack();

	// Inherited functions that are needed for this upgrade
	void IncreaseLevel();
	void OnRangedHit(Entity* target, Entity* projectile);
	void ApplyBoughtUpgrade();

	std::string GetDescription(unsigned int level);

private:
	float m_Radius;
	float m_PercentageDamage;

};

#endif
