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
	Scene* m_pScene = nullptr;
	SceneManager* m_pSceneManager = nullptr;
};


#endif