#include "EnemyFactory.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "GameTracker.h"
#include "GameGUI.h"
#include <istream>
#include <filesystem>

GameTracker::GameTracker()
{
}

GameTracker::~GameTracker()
{
	EventBus::GetInstance().Unsubscribe(this, &GameTracker::sceneChange);
	EventBus::GetInstance().Unsubscribe(this, &GameTracker::levelDone);
	EventBus::GetInstance().Unsubscribe(this, &GameTracker::upgradeBought);

	for (int i = 0; i < m_Levels.size(); i++)
	{
		delete m_Levels.at(i);
	}
}

void GameTracker::Init()
{
	m_GameTime = 0;
	EventBus::GetInstance().Subscribe(this, &GameTracker::sceneChange);
	EventBus::GetInstance().Subscribe(this, &GameTracker::levelDone);
	EventBus::GetInstance().Subscribe(this, &GameTracker::upgradeBought);
}

void GameTracker::printToFile()
{
	int fileCounter = 0;
	while(std::filesystem::exists("game" + std::to_string(fileCounter) + ".txt"))
	{ 
		fileCounter++;
	}
	std::ofstream file;
	file.open("game" + std::to_string(fileCounter) + ".txt");

	file << "Level\tUpgrades\tTime\n";

	for (int i = 0; i < m_Levels.size(); i++)
	{
		file << i + 1 << "\t";
		for (int j = 0; j < m_Levels.at(i)->upgrades.size(); j++)
		{
			file << m_Levels.at(i)->upgrades.at(j) << ";";
		}
		file << "\t" << m_Levels.at(i)->levelTime << "\n";
	}
	file << "Total Time:\t" << m_GameTime << "\t" << "Levels Lasted:\t" << m_Levels.size();
	file.close();
}

void GameTracker::sceneChange(SceneChange* evnt)
{
	if (evnt->m_NewSceneName == "GameScene")
	{
		m_Levels.push_back(new Level);
	}
	if (evnt->m_NewSceneName == "gameOverScene")
	{
		m_Levels.at(m_Levels.size() - 1)->levelTime = EnemyFactory::GetInstance().GetLevelTime();
		m_GameTime = GameGUI::GetInstance().GetTimePlayed();
		printToFile();
	}
}

void GameTracker::levelDone(LevelDone* event)
{
	m_Levels.at(m_Levels.size() - 1)->levelTime = EnemyFactory::GetInstance().GetLevelTime();
}

void GameTracker::upgradeBought(UpgradeBought* evnt)
{
	m_Levels.at(m_Levels.size() - 1)->upgrades.push_back(evnt->name);
}
