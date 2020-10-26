#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../Renderer/MousePicker.h"
#include <set>
#include <unordered_map>

class Entity;
class Renderer;
class AudioEngine;

struct SceneChange;

class SceneManager 
{
public:
	SceneManager();
	~SceneManager();

	// Update
	void Update(double dt);
	void RenderUpdate(double dt);

	// Scene
	Scene* CreateScene(std::string sceneName);
	void SetScenes(unsigned int numScenes, Scene** scene);
	std::vector<Scene*>* GetActiveScenes();
	Scene* GetScene(std::string sceneName) const;
	void ChangeSceneIfTeleported();
	void ResetScene();
	

	// Entity
	void RemoveEntity(Entity* entity, Scene* scene);
	void AddEntity(Entity* entity, Scene* scene);

private:

	// Each scene stores the scene in its starting state, so that the scene can be reset easily.
	std::map<std::string, Scene*> m_Scenes;
	std::vector<Scene*> m_ActiveScenes;

	bool sceneExists(std::string sceneName) const;

	std::string m_SceneToChangeToWhenTeleported = "";
	bool m_ChangeSceneNextFrame = false;
	void changeSceneNextFrame(SceneChange* sceneChangeEvent);
};

#endif
