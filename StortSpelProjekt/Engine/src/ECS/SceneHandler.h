#ifndef SCENEHANDLER_H
#define SCENEHANDLER_H

#include "Scene.h"

class SceneHandler 
{
public:
	SceneHandler();
	~SceneHandler();

	Scene* CreateScene(std::string sceneName);

	Scene* GetScene(std::string sceneName) const;

private:
	std::map<std::string, Scene*> scenes;

	bool SceneExists(std::string sceneName) const;
};

#endif
