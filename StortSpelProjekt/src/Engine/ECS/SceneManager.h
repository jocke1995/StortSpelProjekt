#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "..\Renderer\Renderer.h"

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
	Renderer* renderer;

	std::map<std::string, Scene*> scenes;

	bool SceneExists(std::string sceneName) const;
	void HandleSceneComponents(Scene* scene);
	void ManageComponent(Entity* entity, bool remove);
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

		this->renderer->PrepareCBPerFrame();
		this->renderer->PrepareCBPerScene();

		// -------------------- DEBUG STUFF --------------------
		// Test to change camera to the shadow casting lights cameras
		//auto& tuple = this->renderer->lights[LIGHT_TYPE::SPOT_LIGHT].at(0);
		//BaseCamera* tempCam = std::get<0>(tuple)->GetCamera();
		//this->renderer->ScenePrimaryCamera = tempCam;
		if (this->renderer->ScenePrimaryCamera == nullptr)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "No primary camera was set in scene: %s\n", scene->GetName());
			
			// Todo: Set default camera
		}

		this->renderer->mousePicker->SetPrimaryCamera(renderer->ScenePrimaryCamera);
		scene->SetPrimaryCamera(renderer->ScenePrimaryCamera);
		this->renderer->SetRenderTasksRenderComponents();
		this->renderer->SetRenderTasksPrimaryCamera();

		this->renderer->currActiveScene = scene;
		return;
	}

	Entity* entity = dynamic_cast<Entity*>(input);

	if (entity != nullptr)
	{
		ManageComponent(entity, remove);
		return;
	}
	
	Log::PrintSeverity(Log::Severity::CRITICAL, "The pointer sent to EditScene is neither an Entity or a Scene!\n");
}

#endif
