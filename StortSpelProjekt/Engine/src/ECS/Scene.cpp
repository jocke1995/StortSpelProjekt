#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"
#include "../Memory/PoolAllocator.h"
#include "../Misc/NavMesh.h"

#include "../Renderer/Camera/BaseCamera.h"
Scene::Scene(std::string sceneName)
{
    m_SceneName = sceneName;
    m_pNavMesh = nullptr;
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

    //static int a = 0;
    //if (a == 0)
    //{
        delete m_pNavMesh;
       // a++;
   // }
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
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to remove entity \'%s\' that does not exist in scene: %s\n", entityName, m_SceneName);
        return false;
    }

    PoolAllocator<Entity>::GetInstance().Delete(m_Entities[entityName]);
    m_Entities.erase(entityName);

    m_NrOfEntities--;
    return true;
}

void Scene::CreateNavMesh()
{
    m_pNavMesh = new NavMesh(this);
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

NavMesh* Scene::GetNavMesh()
{
    return m_pNavMesh;
}

void Scene::SetUpdateScene(void(*UpdateScene)(SceneManager*, double dt))
{
    m_UpdateScene = UpdateScene;
}

void Scene::RenderUpdate(SceneManager* sm, double dt)
{
    // Run the scenes specific update function
    m_UpdateScene(sm, dt);

    for (auto pair : m_Entities)
    {
        pair.second->RenderUpdate(dt);
    }
}

void Scene::SetCollisionEntities(const std::vector<Entity*>* collisionEntities)
{
    m_CollisionEntities = *collisionEntities;
}

const std::vector<Entity*>* Scene::GetCollisionEntities() const
{
    return &m_CollisionEntities;
}

void Scene::SetOriginalPosition(float x, float y, float z)
{
    m_OriginalPosition = {x, y, z};
}

void Scene::Update(SceneManager* sm, double dt)
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
