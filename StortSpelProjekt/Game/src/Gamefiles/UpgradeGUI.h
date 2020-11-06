#ifndef UPGRADE_GUI_H
#define UPGRADE_GUI_H

#include <map>
#include <string>
#include <vector>

class Scene;
class Event;
class Texture;
class SceneManager;
struct UForUpgrade;
class UpgradeGUI
{
public:
	static UpgradeGUI& GetInstance();
	~UpgradeGUI();

	void Update(double dt, Scene* scene);
	void CreateMenu(Scene* scene);
	void LoadTextures();

	void SetCurrentScene(Scene* scene);
	void SetSceneMan(SceneManager* sceneManager);
	void SetShown(bool shown);
private:
	UpgradeGUI();

	bool m_Shown = false;
	bool m_Deleted = true;
	bool m_Drawn = false;
	Scene* m_CurrentScene = nullptr;
	SceneManager* m_Sm = nullptr;

	void updateDescription(Scene* scene);
	void showMenu(UForUpgrade* keyPress);
	void getButtonPress(Event* uPress);


	// Textures:
	Texture* m_deviderTexture;
	Texture* m_buttonMintTexture;
	Texture* m_buttonElipseTexture;
	Texture* m_orangeBackgroundTexture;
	Texture* m_yellowGradientTexture;
	Texture* m_orangeGradientTexture;
};


#endif // !UPGRADE_GUI_H
