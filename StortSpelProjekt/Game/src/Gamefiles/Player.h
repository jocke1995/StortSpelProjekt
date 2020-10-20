#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <vector>
#include <string>

class Entity;
class Shop;
class UpgradeManager;

struct UForUpgrade;

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
	Shop* GetShop();

private:
	Player();
	Entity* m_pPlayer;

	// Player entitys upgrade manager
	UpgradeManager* m_pUpgradeManager = nullptr;
	Shop* m_pShop;

	void buyUpgrade(UForUpgrade* evnt);
};


#endif // !PLAYER_H