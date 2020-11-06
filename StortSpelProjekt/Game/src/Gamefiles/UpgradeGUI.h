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
	float2 m_ButtonPos = { 0.47f, 0.206f };
	float m_ButtonYOffset = 0.05f;
	Scene* m_CurrentScene = nullptr;
	SceneManager* m_Sm = nullptr;
	std::map<std::string, int> m_AppliedUpgradeEnums;
	std::vector<std::string> m_ButtonNames;

	void showMenu(UForUpgrade* keyPress);
	//void get
	void makeUpgradeButton(float2 pos, std::string name);
	void updateDescription(Scene* scene);
	void getButtonPress(ButtonPressed* uPress);


	// Textures:
	Texture* m_deviderTexture;
	Texture* m_buttonMintTexture;
	Texture* m_buttonElipseTexture;
	Texture* m_orangeBackgroundTexture;
	Texture* m_yellowGradientTexture;
	Texture* m_orangeGradientTexture;
};


#endif // !UPGRADE_GUI_H
