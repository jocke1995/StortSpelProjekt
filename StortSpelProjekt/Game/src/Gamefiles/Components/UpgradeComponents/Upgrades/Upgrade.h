#ifndef UPGRADE_H
#define UPGRADE_H
#include <string>

// forward declaration
class Entity;

// Types of upgrades, for sorting/call purposes
enum F_UpgradeType
{
	RANGE = (1 << 1),			// range weapon/attack upgrades
	MELEE = (1 << 2),			// melee weapon/attack upgrades
	MOVEMENT = (1 << 3),		// upgrades regarding movement.
	STATS = (1 << 4),			// stat modifications such as hp change.
	ONDAMAGE = (1 << 5),		// upgrades that are "triggered" when damage is taken.
	ENEMYSPECIFIC = (1 << 6)	// upgrades only relevant for enemy entities. example: enemy explode on death)
};

class Upgrade
{
public:
	Upgrade(Entity* parentEntity);
	virtual ~Upgrade();
	
	void SetParentEntity(Entity* parentEntity);

	// setters and getters for private member variables
	void SetName(std::string name);
	std::string GetName() const;
	int GetLevel() const;
	void SetType(unsigned int type);
	unsigned int GetType() const;

	// If dt is needed for an upgrade
	virtual void Update(double dt);
	virtual void RenderUpdate(double dt);

	// Below are all functions needed by upgrades. Some will be used by several upgrades and others might be unique.
	// This way you can call functions that range over several "types" but might be called on in similar situations (such as melee/range OnHit).
	// Definitions are to be implemented in the separate upgrade classes. Add more as needed.

	// This function is supposed to increase the level of the upgrade, and to make sure that appropriate changes are made in the upgrade to reflect the level of the upgrade.
	virtual void IncreaseLevel();
	// upgrades that are triggered on hit (ex: explosive, poison)
	virtual void OnHit();
	// specific for ranged hits
	virtual void OnRangedHit();
	// specific for melee hits
	virtual void OnMeleeHit();
	// upgrades that are triggered when taking damage
	virtual void OnDamage();
	// upgrades that are applied immediately, for example apply stat when bought in shop
	virtual void OnPickUp();
	// upgrades triggered on death
	virtual void OnDeath();
	// upgrades that affect the flight pattern (ex: gravity, speed)
	virtual void RangedFlight();
	// upgrades that modify the ranged attack (ex: multiple projectiles)
	virtual void RangedModifier();
	// upgrades that applies/adds stats (ex: more hp, more stamina)
	virtual void ApplyStat();

protected:
	// 
	Entity* m_pParentEntity;
	// Name of the upgrade, for ease of access in shop or upgrade handlers
	std::string m_Name = "";
	// What level the upgrade is, for keeping track of stacking of upgrades and price
	int m_Level = 1;
	// What type of upgrade this is, for ease of sorting or making calls to certain types
	unsigned int m_Type = 0;
};

#endif
