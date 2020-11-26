#ifndef UPGRADE_GUI_H
#define UPGRADE_GUI_H

#include <map>
#include <string>
#include <vector>
#include "EngineMath.h"

class Scene;
class Event;
class Texture;
class SceneManager;
class Font;
struct UForUpgrade;
struct ButtonPressed;
class UpgradeGUI
{
public:
	static UpgradeGUI& GetInstance();
	~UpgradeGUI();

	void Update(double dt, Scene* scene);
	void CreateMenu(Scene* scene);
	void Init();

	void SetCurrentScene(Scene* scene);
	void SetSceneMan(SceneManager* sceneManager);
	void SetShown(bool shown);

	void SetCreateUpgradeButtons();
private:
	UpgradeGUI();

	// are we supposed to show the menu?
	bool m_Shown = false;
	// Is the menu currently drawn on screen?
	bool m_Drawn = false;
	// Has the menu already been deleted?
	bool m_Deleted = true;
	// Has the description already been deleted?
	bool m_DescriptionDeleted = true;
	// Should we cycle through the UpgradeButtons?
	bool m_LoopButtons = false;
	// Value for buttons.
	float2 m_ButtonPos = { 0.54f, 0.141f };
	float m_ButtonYOffset = 0.048f;
	// How many times the menu can be filled with buttons.
	// Used for keeping track of how many times we need to cycle
	// the menu to show all buttons.
	int m_ButtonsMultipleTen = 0;
	// How many times we have filled the menu with buttons.
	// Used for cycling through buttons.
	int m_TimesFilledMenu = 0;
	std::string m_CurrentDescription = "";
	Scene* m_pCurrentScene = nullptr;
	SceneManager* m_pSm = nullptr;
	// Used to get names of upgrades for our buttons.
	std::map<std::string, int> m_AppliedUpgradeEnums;
	std::vector<std::string> m_ButtonNames;

	void showMenu(UForUpgrade* keyPress);
	//void get
	void makeUpgradeButton(float2 pos, std::string name);
	void getButtonPress(ButtonPressed* event);
	void updateDescription(int level);

	// Fonts
	Font* m_pArial = nullptr;

	// Textures:
	Texture* m_pBoardBackgroundTexture;
	Texture* m_pButtonParchment;
	Texture* m_pDescriptionParchment;
};


#endif // !UPGRADE_GUI_H
