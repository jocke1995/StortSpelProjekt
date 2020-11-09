#ifndef MAINMENUHANDLER_H
#define MAINMENUHANDLER_H
class Scene;
class SceneManager;

class MainMenuHandler
{
public:
	static MainMenuHandler& GetInstance();
	virtual ~MainMenuHandler();
	Scene* CreateScene(SceneManager* sm);
	Scene* GetScene();

private:
	MainMenuHandler();

	//OPTION MENU
	void createOptionScene();
	Scene* m_pOptionScene = nullptr;
	bool m_RestartSetting;


	Scene* m_pScene = nullptr;
	SceneManager* m_pSceneManager = nullptr;
};


#endif