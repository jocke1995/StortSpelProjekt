#ifndef GAME_TRACKER_H
#define GAME_TRACKER_H

#include "EngineMath.h"
#include "Core.h"
#include "Misc/EngineRand.h"
#include "Misc/EngineRand.h"
#include <map>
#include <vector>

struct SceneChange;
struct UpgradeBought;
struct LevelDone;

struct Level
{
	std::vector<std::string> upgrades;
	int levelTime;
};

class GameTracker
{
public:
	GameTracker();
	~GameTracker();

	void Init();

private:
	void printToFile();

	void sceneChange(SceneChange* evnt);
	void levelDone(LevelDone* evnt);
	void upgradeBought(UpgradeBought* evnt);

	std::vector<Level*> m_Levels;
	int m_GameTime;
};

#endif