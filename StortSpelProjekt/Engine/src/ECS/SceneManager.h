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
	void SetSceneToDraw(Scene* scene);
private:
	Renderer* m_pRenderer;

	std::map<std::string, Scene*> m_pScenes;

	bool sceneExists(std::string sceneName) const;
	void executeCopyOnDemand();
	void resetScene();
};

#endif
