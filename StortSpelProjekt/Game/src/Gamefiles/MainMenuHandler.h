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

	void createOptionScene();

	Scene* m_pScene = nullptr;
	Scene* m_pOptionScene = nullptr;
	SceneManager* m_pSceneManager = nullptr;
};


#endif