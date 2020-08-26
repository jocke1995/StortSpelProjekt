#ifndef SCENEHANDLER_H
#define SCENEHANDLER_H

#include "Scene.h"
#include "..\Renderer\Renderer.h"

class SceneHandler 
{
public:
	SceneHandler(Renderer *r);
	~SceneHandler();

	Scene* CreateScene(std::string sceneName);

	Scene* GetScene(std::string sceneName) const;
	
	// Input is either an Entity or a Scene
	template<class T>
	void ManageScene(T* input);
private:
	Renderer* renderer;

	std::map<std::string, Scene*> scenes;

	bool SceneExists(std::string sceneName) const;
	
};

template<class T>
inline void SceneHandler::ManageScene(T* input)
{
	if (dynamic_cast<Scene*>(input))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "New Scene!\n");
		
		//renderer->ResetScene();
	}
	else if (dynamic_cast<Entity*>(input))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Added Entity!\n");
	}
}


#endif
