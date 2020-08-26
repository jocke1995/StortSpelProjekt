#include "SceneHandler.h"

SceneHandler::SceneHandler(Renderer* r)
{
	this->renderer = r;
}

SceneHandler::~SceneHandler()
{
    for (auto pair : this->scenes)
    {
        delete pair.second;
    }
    this->scenes.clear();
}

Scene* SceneHandler::CreateScene(std::string sceneName)
{
    if (this->SceneExists(sceneName))
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "A scene with the name: \'%s\' already exists.\n", sceneName.c_str());
        return nullptr;
    }

    // Create Scene and return it
    this->scenes[sceneName] = new Scene(sceneName);
    return this->scenes[sceneName];
}

Scene* SceneHandler::GetScene(std::string sceneName) const
{
    if (this->SceneExists(sceneName))
    {
        return this->scenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

bool SceneHandler::SceneExists(std::string sceneName) const
{
    for (auto pair : this->scenes)
    {
        // A Scene with this name already exists
        if (pair.first == sceneName)
        {
            return true;
        }
    }

    return false;
}
