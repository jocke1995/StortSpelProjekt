#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"
#include "../Memory/PoolAllocator.h"
#include "../Misc/NavMesh.h"

#include "../Renderer/Camera/BaseCamera.h"

void DefaultUpdateScene(SceneManager* sm, double dt)
{

}

void DefaultInitScene(Scene* scene)
{
}

Scene::Scene(std::string sceneName)
{
    m_SceneName = sceneName;
    m_pNavMesh = nullptr;

    m_pUpdateScene = &DefaultUpdateScene;
    m_pOnInit = &DefaultInitScene;
}

Scene::~Scene()
{
    for (auto pair : m_EntitiesToKeep)
    {
        if (pair.second != nullptr)
        {
            PoolAllocator<Entity>::GetInstance().Delete(pair.second);
        }
    }

    delete m_pNavMesh;
}

Entity* Scene::AddEntityFromOther(Entity* other)
{
    if (EntityExists(other->GetName()) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "AddEntityFromOther: Trying to add two Entities with the same name \'%s\' into scene: %s\n", other->GetName().c_str(), m_SceneName.c_str());
        return nullptr;
    }

    m_EntitiesToKeep[other->GetName()] = other;

    m_NrOfEntities++;
    return other;
}

// Returns false if the entity couldn't be created
Entity* Scene::AddEntity(std::string entityName)
{
    if (EntityExists(entityName) == true)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add two Entities with the same name \'%s\' into scene: %s\n", entityName.c_str(), m_SceneName.c_str());
        return nullptr;
    }

    m_EntitiesToKeep[entityName] = PoolAllocator<Entity>::GetInstance().Allocate(entityName);
    m_NrOfEntities++;

    return m_EntitiesToKeep[entityName];
}

void Scene::InitDynamicEntity(Entity* ent)
{
    ent->SetEntityState(true);
    ent->OnInitScene();
}

bool Scene::RemoveEntity(std::string entityName)
{
    if (!EntityExists(entityName))
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to remove entity \'%s\' that does not exist in scene: %s\n", entityName.c_str(), m_SceneName.c_str());
        return false;
    }

    Entity* ent = m_EntitiesToKeep[entityName];

    PoolAllocator<Entity>::GetInstance().Delete(ent);
    m_EntitiesToKeep.erase(entityName);
    for (int i = 0; i < m_CollisionEntities.size(); ++i)
    {
        Entity* entity = m_CollisionEntities[i];
        if (entity->GetID() == ent->GetID())
        {
            m_CollisionEntities.erase(m_CollisionEntities.begin() + i);
        }
    }

    m_NrOfEntities--;

    for (int i = 0; i < m_CollisionEntities.size(); ++i)
    {
        Entity* entity = m_CollisionEntities[i];
        if (entity->GetID() == ent->GetID())
        {
            m_CollisionEntities.erase(m_CollisionEntities.begin() + i);
        }
    }

    return true;
}

NavMesh* Scene::CreateNavMesh(const std::string& type)
{
    if (m_pNavMesh == nullptr)
    {
        m_pNavMesh = new NavMesh(this, type);
    }
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

    Log::PrintSeverity(Log::Severity::WARNING, "No Entity with name: \'%s\' was found.\n", entityName.c_str());
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

void Scene::SetUpdateScene(void(*UpdateScene)(SceneManager*, double dt))
{
    m_pUpdateScene = UpdateScene;
}

void Scene::RenderUpdate(SceneManager* sm, double dt)
{
    // Run the scenes specific update function
    m_pUpdateScene(sm, dt);

    for (auto pair : m_EntitiesToKeep)
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

void Scene::OnInit()
{
    m_pOnInit(this);
}

void Scene::SetOnInit(void(*OnInit)(Scene*))
{
    m_pOnInit = OnInit;
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

void Scene::ResetNavMesh()
{
    if (m_pNavMesh != nullptr)
    {
        delete m_pNavMesh;
        m_pNavMesh = nullptr;
    }
}