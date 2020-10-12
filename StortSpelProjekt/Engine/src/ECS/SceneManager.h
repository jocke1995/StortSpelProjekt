#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../Renderer/MousePicker.h"
#include <set>
#include <unordered_map>

class Entity;
class Renderer;
class AudioEngine;

class SceneManager 
{
public:
	SceneManager();
	~SceneManager();

	void Update(double dt);
	void RenderUpdate(double dt);

	Scene* CreateScene(std::string sceneName);

	std::vector<Scene*>* GetActiveScenes();
	Scene* GetScene(std::string sceneName) const;

	void RemoveEntity(Entity* entity, Scene* scene);
	void AddEntity(Entity* entity, Scene* scene);
	void SetScenes(unsigned int numScenes, Scene** scene);
	void LoadScene(Scene* scene);
	void UnloadScene(Scene* scene);
	void ResetScene();
private:
	std::map<std::string, Scene*> m_Scenes;
	std::vector<Scene*> m_ActiveScenes;
	std::set<Scene*> m_LoadedScenes;

	std::unordered_map<Entity*, bool> m_IsEntityInited;

	bool sceneExists(std::string sceneName) const;
};

#endif
