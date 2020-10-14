#include "Shop.h"

Shop::Shop()
{
	m_pUpgradeManager = Player::GetInstance().GetUpgradeManager();
	m_InvSize = 3;
}

Shop::~Shop()
{

}

void Shop::ApplyUppgrade(std::string name)
{
	m_pUpgradeManager->ApplyUpgrade(name);
}

void Shop::checkExisting(std::string name)
{
	m_pUpgradeManager;
}
