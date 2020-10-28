#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"
#include "../Memory/PoolAllocator.h"
#include "../Misc/NavMesh.h"

#include "../Renderer/Camera/BaseCamera.h"

void DefaultUpdateScene(SceneManager* sm, double dt)
{

}

Scene::Scene(std::string sceneName)
{
    m_SceneName = sceneName;
    m_pNavMesh = nullptr;

    m_UpdateScene = &DefaultUpdateScene;
}

Scene::~Scene()
{
    for (auto pair : m_EntitiesToKeep)
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

    delete m_pNavMesh;
}

Entity* Scene::AddEntityFromOther(Entity* other)
{
    if (EntityExists(other->GetName()) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "AddEntityFromOther: Trying to add two components with the same name \'%s\' into scene: %s\n", other->GetName(), m_SceneName);
        return nullptr;
    }

    m_EntitiesToKeep[other->GetName()] = other;
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

    m_EntitiesToKeep[entityName] = PoolAllocator<Entity>::GetInstance().Allocate(entityName);
    m_EntitiesToKeep[entityName]->IncrementRefCount();
    m_NrOfEntities++;

    return m_EntitiesToKeep[entityName];
}

bool Scene::RemoveEntity(std::string entityName)
{
    if (!EntityExists(entityName))
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to remove entity \'%s\' that does not exist in scene: %s\n", entityName, m_SceneName);
        return false;
    }

    PoolAllocator<Entity>::GetInstance().Delete(m_EntitiesToKeep[entityName]);
    m_EntitiesToKeep.erase(entityName);

    m_NrOfEntities--;
    return true;
}

NavMesh* Scene::CreateNavMesh(const std::string& type)
{
    m_pNavMesh = new NavMesh(this, type);
    return m_pNavMesh;
}

void Scene::SetPrimaryCamera(BaseCamera* primaryCamera)
{
    m_pPrimaryCamera = primaryCamera;
}

Entity* Scene::GetEntity(std::string entityName)
{
    if (EntityExists(entityName))
    {
        return m_EntitiesToKeep.at(entityName);
    }

    Log::PrintSeverity(Log::Severity::CRITICAL, "No Entity with name: \'%s\' was found.\n", entityName.c_str());
    return nullptr;
}

const std::map<std::string, Entity*>* Scene::GetEntities() const
{
    return &m_EntitiesToKeep;
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

NavMesh* Scene::GetNavMesh()
{
    return m_pNavMesh;
}

void Scene::RenderUpdate(double dt)
{
    m_UpdateScene = UpdateScene;
}

void Scene::RenderUpdate(SceneManager* sm, double dt)
{
    // Run the scenes specific update function
    m_UpdateScene(sm, dt);

    for (auto pair : m_EntitiesToKeep)
    {
        pair.second->RenderUpdate(dt);
    }
}

void Scene::Update(double dt)
{
    m_CollisionEntities = *collisionEntities;
}

const std::vector<Entity*>* Scene::GetCollisionEntities() const
{
    return &m_CollisionEntities;
}

void Scene::Update(SceneManager* sm, double dt)
{
    for (auto pair : m_EntitiesToKeep)
    {
        pair.second->Update(dt);
    }
}

bool Scene::EntityExists(std::string entityName) const
{
    for (auto pair : m_EntitiesToKeep)
    {
        // An entity with this m_Name already exists
        if (pair.first == entityName)
        {
            return true;
        }
    }

    return false;
}
