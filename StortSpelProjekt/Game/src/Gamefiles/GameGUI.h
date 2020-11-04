#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <map>
#include <string>
#include <vector>

class GameGUI
{
public:
	GameGUI();

	void Update(double dt, Scene* scene);

private:
	int m_OldCurrHealth;
	int m_OldMaxHealth;
	int m_OldMoney;
	float2 m_HealthSize;

	void updateHealth(Scene* scene);
};


#endif // !GAME_GUI_H