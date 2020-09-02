#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../Renderer/MousePicker.h"

class Entity;
class Renderer;

class SceneManager 
{
public:
	SceneManager(Renderer *r);
	~SceneManager();

	Scene* CreateScene(std::string sceneName);

	Scene* GetScene(std::string sceneName) const;

	template<class T>
	void EditScene(T* input, bool remove = false);
private:
	Renderer* m_pRenderer;

	std::map<std::string, Scene*> m_pScenes;

	bool SceneExists(std::string sceneName) const;
	void HandleSceneComponents(Scene* scene);
	void ManageComponent(Entity* entity, bool remove);
	void ExecuteCopyOnDemand();
	void ResetScene();
};

// Edit takes in either Scene* to set, or an Entity* to add/remove
template<class T>
void SceneManager::EditScene(T* input, bool remove)
{
	Scene* scene = dynamic_cast<Scene*>(input);
	
	if (scene != nullptr)
	{
		ResetScene();
		HandleSceneComponents(scene);

		m_pRenderer->prepareCBPerFrame();
		m_pRenderer->prepareCBPerScene();

		// -------------------- DEBUG STUFF --------------------
		// Test to change m_pCamera to the shadow casting m_lights cameras
		//auto& tuple = m_pRenderer->m_lights[LIGHT_TYPE::SPOT_LIGHT].at(0);
		//BaseCamera* tempCam = std::get<0>(tuple)->GetCamera();
		//m_pRenderer->ScenePrimaryCamera = tempCam;
		if (m_pRenderer->m_pScenePrimaryCamera == nullptr)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "No primary camera was set in scene: %s\n", scene->GetName());
			
			// Todo: Set default m_pCamera
		}

		m_pRenderer->m_pMousePicker->SetPrimaryCamera(m_pRenderer->m_pScenePrimaryCamera);
		scene->SetPrimaryCamera(m_pRenderer->m_pScenePrimaryCamera);
		m_pRenderer->setRenderTasksRenderComponents();
		m_pRenderer->setRenderTasksPrimaryCamera();

		m_pRenderer->m_pCurrActiveScene = scene;

		ExecuteCopyOnDemand();
		return;
	}

	Entity* entity = dynamic_cast<Entity*>(input);

	if (entity != nullptr)
	{
		ManageComponent(entity, remove);
		ExecuteCopyOnDemand();
		return;
	}
	
	Log::PrintSeverity(Log::Severity::CRITICAL, "The pointer sent to EditScene is neither an Entity or a Scene!\n");
}

#endif
