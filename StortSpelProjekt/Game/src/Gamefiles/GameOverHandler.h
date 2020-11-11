#ifndef GAMEOVERHANDLER_H
#define GAMEOVERHANDLER_H
class Scene;
class SceneManager;

class GameOverHandler
{
public:
	static GameOverHandler& GetInstance();
	virtual ~GameOverHandler();
	Scene* CreateScene(SceneManager* sm);
	Scene* GetScene();
private:
	GameOverHandler();
	Scene* m_pScene = nullptr;
	SceneManager* m_pSceneManager = nullptr;
};

#endif