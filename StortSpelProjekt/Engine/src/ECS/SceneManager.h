#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../Renderer/MousePicker.h"
#include <set>

class Entity;
class Renderer;
class AudioEngine;

class SceneManager 
{
public:
	SceneManager(Renderer* r);
	~SceneManager();

	void Update(double dt);
	void RenderUpdate(double dt);

	Scene* CreateScene(std::string sceneName);

	Scene* GetScene(std::string sceneName) const;

	void RemoveEntity(Entity* entity);
	void AddEntity(Entity* entity);
	void SetScene(unsigned int numScenes, Scene** scene);
	void LoadScene(Scene* scene);
	void UnloadScene(Scene* scene);
	void ResetScene();
private:
	Renderer* m_pRenderer;

	std::map<std::string, Scene*> m_pScenes;
	std::vector<Scene*> m_ActiveScenes;
	std::set<Scene*> m_LoadedScenes;

	bool sceneExists(std::string sceneName) const;
	void executeCopyOnDemand();
	
};

#endif
