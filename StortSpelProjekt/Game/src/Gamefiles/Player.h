#ifndef PLAYER_H
#define PLAYER_H

#include "Components/UpgradeComponents/UpgradeComponent.h"
#include <map>
#include <vector>
#include <string>

// All uppgrades
#include "Components/UpgradeComponents/Upgrades/UpgradeRangeTest.h"

enum E_RangeName
{
	RANGE_TEST = 1,

};
class Entity;
class Player
{
public:
	static Player& GetInstance();
	~Player();

	void SetPlayer(Entity* player);
	Entity* GetPlayer();

	void ApplyUpgrade(std::string name);
	void ApplyRangeUpgrades(Entity* ent);

private:
	Player();
	Entity* m_pPlayer;
	// Contains all uppgrades that the player can buy
	std::map<std::string, Upgrade*> m_AllAvailablePlayerUpgrades;
	// Contains all uppgrades that the player can buy that will be applied to projectile entities
	std::vector<std::string> m_AllAvailablePlayerProjectileUpgrades;
	// Contains all player uppgrades that will be applied to porjectile Entities
	std::vector<std::string> m_AllAppliedProjectileUpgrades;
	// Contains enums for all rangeUpgrades. 
	// Used when getting NEW rgange upgrades for projectile entities.
	std::map<std::string, int> m_RangeUpgradeEnmus;

	void fillUppgradeList();
	// Returns true if it is of type RANGE and should then be put on the Projectile entity
	bool checkIfRangeUpgrade(std::string name);
	// returns true if it is of a type that should be applied on the player entity
	bool checkIfPlayerEntityUpgrade(std::string name);
	// Returns a new rangeUpgrade so that every projectile entity has it's own rangeUpgrade.
	Upgrade* RangeUpgrade(std::string name, Entity* ent);
};


#endif // !PLAYER_H