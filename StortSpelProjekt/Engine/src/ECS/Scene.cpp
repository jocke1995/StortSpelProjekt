#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"
#include "../Renderer/BaseCamera.h"
Scene::Scene(std::string sceneName)
{
    this->sceneName = sceneName;
}

Scene::~Scene()
{
    for (auto pair : this->entities)
    {
        if (pair.second != nullptr)
        {
            if (pair.second->GetRefCount() == 1)
            {
                delete pair.second;
            }
            pair.second->DecrementRefCount();
        }
    }
    this->entities.clear();
}

Entity* Scene::AddEntityFromOther(Entity* other)
{
    if (this->EntityExists(other->GetName()) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add two components with the same name \'%s\' into scene: %s\n", other->GetName(), this->sceneName);
        return nullptr;
    }

    this->entities[other->GetName()] = other;
    other->IncrementRefCount();

    this->nrOfEntities++;
    return other;
}

// Returns false if the entity couldn't be created
Entity* Scene::AddEntity(std::string entityName)
{
    if (this->EntityExists(entityName) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add two components with the same name \'%s\' into scene: %s\n", entityName, this->sceneName);
        return nullptr;
    }

    this->entities[entityName] = new Entity(entityName);
    this->nrOfEntities++;
    return this->entities[entityName];
}

bool Scene::RemoveEntity(std::string entityName)
{
    if (this->EntityExists(entityName) == false)
    {
        return false;
    }

    delete this->entities[entityName];
    this->entities.erase(entityName);

    this->nrOfEntities--;
    return true;
}

void Scene::SetPrimaryCamera(BaseCamera* primaryCamera)
{
    this->primaryCamera = primaryCamera;
}

Entity* Scene::GetEntity(std::string entityName)
{
    if (this->EntityExists(entityName))
    {
        return this->entities.at(entityName);
    }

    Log::PrintSeverity(Log::Severity::CRITICAL, "No Entity with name: \'%s\' was found.\n", entityName.c_str());
    return nullptr;
}

const std::map<std::string, Entity*>* Scene::GetEntities() const
{
	return &this->entities;
}

unsigned int Scene::GetNrOfEntites() const
{
    return this->nrOfEntities;
}

BaseCamera* Scene::GetMainCamera() const
{
	return this->primaryCamera;
}

std::string Scene::GetName() const
{
    return this->sceneName;
}

void Scene::UpdateScene(double dt)
{
    for (auto pair : this->entities)
    {
        pair.second->Update(dt);
    }
}

bool Scene::EntityExists(std::string entityName) const
{
    for (auto pair : this->entities)
    {
        // An entity with this name already exists
        if (pair.first == entityName)
        {
            return true;
        }
    }
    
    return false;
}
