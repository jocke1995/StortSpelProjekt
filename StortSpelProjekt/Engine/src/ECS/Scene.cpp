#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"
#include "../Renderer/BaseCamera.h"
#include "../Memory/PoolAllocator.h"
Scene::Scene(std::string sceneName)
{
    m_SceneName = sceneName;
}

Scene::~Scene()
{
    for (auto pair : m_Entities)
    {
        if (pair.second != nullptr)
        {
            pair.second->DecrementRefCount();
            if (pair.second->GetRefCount() == 0)
            {
                PoolAllocator<Entity>::GetInstance().Delete(pair.second);
            }
        }
    }
}

Entity* Scene::AddEntityFromOther(Entity* other)
{
    if (EntityExists(other->GetName()) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "AddEntityFromOther: Trying to add two components with the same name \'%s\' into scene: %s\n", other->GetName(), m_SceneName);
        return nullptr;
    }

    m_Entities[other->GetName()] = other;
    other->IncrementRefCount();

    m_NrOfEntities++;
    return other;
}

// Returns false if the entity couldn't be created
Entity* Scene::AddEntity(std::string entityName)
{
    if (EntityExists(entityName) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add two components with the same name \'%s\' into scene: %s\n", entityName, m_SceneName);
        return nullptr;
    }

    m_Entities[entityName] = PoolAllocator<Entity>::GetInstance().Allocate(entityName);
    m_Entities[entityName]->IncrementRefCount();
    m_NrOfEntities++;
    return m_Entities[entityName];
}

bool Scene::RemoveEntity(std::string entityName)
{
    if (!EntityExists(entityName))
    {
        return false;
    }

    PoolAllocator<Entity>::GetInstance().Delete(m_Entities[entityName]);
    m_Entities.erase(entityName);

    m_NrOfEntities--;
    return true;
}

void Scene::SetPrimaryCamera(BaseCamera* primaryCamera)
{
    m_pPrimaryCamera = primaryCamera;
}

Entity* Scene::GetEntity(std::string entityName)
{
    if (EntityExists(entityName))
    {
        return m_Entities.at(entityName);
    }

    Log::PrintSeverity(Log::Severity::CRITICAL, "No Entity with name: \'%s\' was found.\n", entityName.c_str());
    return nullptr;
}

const std::map<std::string, Entity*>* Scene::GetEntities() const
{
	return &m_Entities;
}

unsigned int Scene::GetNrOfEntites() const
{
    return m_NrOfEntities;
}

BaseCamera* Scene::GetMainCamera() const
{
	return m_pPrimaryCamera;
}

std::string Scene::GetName() const
{
    return m_SceneName;
}

void Scene::RenderUpdate(double dt)
{
    for (auto pair : m_Entities)
    {
        pair.second->RenderUpdate(dt);
    }
}

void Scene::Update(double dt)
{
    for (auto pair : m_Entities)
    {
        pair.second->Update(dt);
    }
}

bool Scene::EntityExists(std::string entityName) const
{
    for (auto pair : m_Entities)
    {
        // An entity with this m_Name already exists
        if (pair.first == entityName)
        {
            return true;
        }
    }
    
    return false;
}
