#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <map>
#include <string>
#include <vector>

struct SceneChange;

class GameGUI
{
public:
	static GameGUI& GetInstance();

	void Update(double dt, Scene* scene);

	//Gets for how long the player has played the current round as a whole number
	int GetTimePlayed();

private:
	GameGUI();

	int m_TimePlayed; //Holds the time as int to allow higher count and not lose precision with higher count
	double m_TimePlayedTimer; //Count to 1 to increase TimePlayeds

	int m_OldHealth;
	int m_OldMaxHealth;
	int m_OldMoney;
	float m_OldHealthLength;
	Scene* m_pOldScene;

	void updateHealth(Scene* scene);
	void reset(Scene* scene);
	void sceneChange(SceneChange* evnt);
};


#endif // !GAME_GUI_H