#include "Player.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

Player::Player()
{
	
}

Player& Player::GetInstance()
{
	static Player instance;
	return instance;
}

Player::~Player()
{
	delete m_pUpgradeManager;
}

void Player::SetPlayer(Entity* player)
{
	m_pPlayer = player;
	m_pUpgradeManager = new UpgradeManager(player);
}

Entity* Player::GetPlayer()
{
	return m_pPlayer;
}

UpgradeManager* Player::GetUpgradeManager()
{
	return m_pUpgradeManager;
}
