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

	void SetPlayer(Entity* player);
	
	Entity* GetPlayer();
	UpgradeManager* GetUpgradeManager();

private:
	Player();
	Entity* m_pPlayer;

	UpgradeManager* m_pUpgradeManager = nullptr;

};


#endif // !PLAYER_H