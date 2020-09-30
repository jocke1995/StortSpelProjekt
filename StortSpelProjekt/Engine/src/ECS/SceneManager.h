#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "../Renderer/MousePicker.h"

class Entity;
class Renderer;
class AudioEngine;

class SceneManager 
{
public:
	SceneManager(Renderer* r);
	~SceneManager();

	Scene* CreateScene(std::string sceneName);

	Scene* GetScene(std::string sceneName) const;

	void RemoveEntity(Entity* entity);
	void AddEntity(Entity* entity);
	void SetScene(Scene* scene);
	void ResetScene();
private:
	Renderer* m_pRenderer;

	std::map<std::string, Scene*> m_pScenes;
	Scene* m_pActiveScene = nullptr;

	bool sceneExists(std::string sceneName) const;
	void executeCopyOnDemand();
	
};

#endif
