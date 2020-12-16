#ifndef CREDITSSCENEHANDLER
#define CREDITSSCENEHANDLER

class Scene;
class SceneManager;

class CreditsSceneHandler
{
public:
	static CreditsSceneHandler& GetInstance();
	virtual ~CreditsSceneHandler();
	Scene* CreateScene(SceneManager* sm);
	Scene* GetScene();

private:
	CreditsSceneHandler();


	Scene* m_pScene = nullptr;
	SceneManager* m_pSceneManager = nullptr;
};

#endif