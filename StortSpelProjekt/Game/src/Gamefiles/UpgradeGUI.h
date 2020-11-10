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
private:
	UpgradeGUI();

	bool m_Shown = false;
	bool m_Deleted = true;
	bool m_Drawn = false;
	bool m_DescriptionDeleted = true;
	float2 m_ButtonPos = { 0.47f, 0.211f };
	float m_ButtonYOffset = 0.048f;
	// How many times the menu can be filled with buttons.
	// Used for keeping track of how many times we need to cycle
	// the menu to show all buttons.
	int m_ButtonsMultipleTen = 0;
	// How many times we have filled the menu with buttons.
	// Used for cycling through buttons.
	int m_TimesFilledMenu = 0;
	std::string m_CurrentDescription = "";
	Scene* m_CurrentScene = nullptr;
	SceneManager* m_Sm = nullptr;
	std::map<std::string, int> m_AppliedUpgradeEnums;
	std::vector<std::string> m_ButtonNames;

	void showMenu(UForUpgrade* keyPress);
	//void get
	void makeUpgradeButton(float2 pos, std::string name);
	void getButtonPress(ButtonPressed* event);
	void updateDescription();



	// Textures:
	Texture* m_deviderTexture;
	Texture* m_buttonMintTexture;
	Texture* m_buttonElipseTexture;
	Texture* m_orangeBackgroundTexture;
	Texture* m_yellowGradientTexture;
	Texture* m_orangeGradientTexture;
	Texture* m_boardBackgroundTexture;
	Texture* m_buttonParchment;
	Texture* m_DescriptionParchment;
};


#endif // !UPGRADE_GUI_H
