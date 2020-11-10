#ifndef UPGRADEPOTIONOFSTRENGTH_H
#define UPGRADEPOTIONOFSTRENGTH_H
#include "Upgrade.h"
struct LevelDone;
class UpgradePotionOfStrength : public Upgrade
{
public:
	UpgradePotionOfStrength(Entity* parent);
	~UpgradePotionOfStrength();

	virtual void IncreaseLevel() override;
	virtual void ApplyStat() override;
protected:
	// The changes made to the player must be saved so they can be reversed correctly later.
	// For example (dmg + otherBoost) * thisBoost != dmg * thisBoost + otherBoost

	float m_MeleeChange;
	float m_RangeChange;
	bool m_IsActive = false;
private:
	void onLevelEnd(LevelDone* evnt);
};

#endif