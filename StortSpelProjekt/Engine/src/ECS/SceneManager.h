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
	SceneManager(Renderer* r);
	~SceneManager();

	void Update(double dt);
	void RenderUpdate(double dt);

	Scene* CreateScene(std::string sceneName);

	const std::vector<Scene*>* GetActiveScenes() const;
	Scene* GetScene(std::string sceneName) const;

	void RemoveEntity(Entity* entity, Scene* scene);
	void AddEntity(Entity* entity, Scene* scene);
	void SetScenes(unsigned int numScenes, Scene** scene);
	void LoadScene(Scene* scene);
	void UnloadScene(Scene* scene);
	void ResetScene();
private:
	Renderer* m_pRenderer;

	std::map<std::string, Scene*> m_pScenes;
	std::vector<Scene*> m_ActiveScenes;
	std::set<Scene*> m_LoadedScenes;

	std::unordered_map<Entity*, bool> m_IsEntityInited;

	bool sceneExists(std::string sceneName) const;
	void executeCopyOnDemand();
	
};

#endif
