#ifndef PLAYER_H
#define PLAYER_H

#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "UpgradeManager.h"
#include <map>
#include <vector>
#include <string>

class Entity;
class Player
{
public:
	static Player& GetInstance();
	~Player();

	// Later player, and all its components, will be made in this class.
	// But for now as not to break testScenes and because the task was Upgrades
	// player is set in main.
	void SetPlayer(Entity* player);

	Entity* GetPlayer();
	// Get the manager so we can use it in places such as Shop or RangeComponent.
	UpgradeManager* GetUpgradeManager();

private:
	Player();
	Entity* m_pPlayer;

	// Player entitys upgrade manager
	UpgradeManager* m_pUpgradeManager = nullptr;

};


#endif // !PLAYER_H