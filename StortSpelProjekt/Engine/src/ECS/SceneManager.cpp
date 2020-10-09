#include "stdafx.h"
#include "SceneManager.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"
#include "../Physics/Physics.h"

// Renderer
#include "../Renderer/CommandInterface.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"
#include "../Renderer/ShadowInfo.h"
#include "../Renderer/ViewPool.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/Mesh.h"

// CopyTasks
#include "../Renderer/DX12Tasks/CopyOnDemandTask.h"
#include "../Renderer/DX12Tasks/CopyPerFrameTask.h"

// RenderTasks
// WireFrame (currently only used for debugging boundingBoxes
#include "../Renderer/DX12Tasks/WireframeRenderTask.h"
#include "../Renderer/DX12Tasks/ShadowRenderTask.h"
#include "../Renderer/BoundingBoxPool.h"

// Components
#include "../ECS/Components/Lights/DirectionalLightComponent.h"
#include "../ECS/Components/Lights/PointLightComponent.h"
#include "../ECS/Components/Lights/SpotLightComponent.h"
#include "../ECS/Entity.h"

SceneManager::SceneManager()
{
	m_ActiveScenes.reserve(2);
}

SceneManager::~SceneManager()
{
	// Unload all active scenes
	for (auto scene : m_ActiveScenes)
	{
		UnloadScene(scene);
	}

	for (auto pair : m_pScenes)
	{
		delete pair.second;
	}

    m_pScenes.clear();
}

void SceneManager::Update(double dt)
{
	// Update scenes
	for (auto scene : m_ActiveScenes)
	{
		scene->Update(dt);
	}
}

void SceneManager::RenderUpdate(double dt)
{
	// Update scenes (Render)
	for (auto scene : m_ActiveScenes)
	{
		scene->RenderUpdate(dt);
	}

	// Renderer updates some stuff
	Renderer::GetInstance().RenderUpdate(dt);
}

Scene* SceneManager::CreateScene(std::string sceneName)
{
    if (sceneExists(sceneName))
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "A scene with the name: \'%s\' already exists.\n", sceneName.c_str());
        return nullptr;
    }

    // Create Scene and return it
    m_pScenes[sceneName] = new Scene(sceneName);
    return m_pScenes[sceneName];
}

std::vector<Scene*>* SceneManager::GetActiveScenes()
{
	return &m_ActiveScenes;
}

Scene* SceneManager::GetScene(std::string sceneName) const
{
    if (sceneExists(sceneName))
    {
        return m_pScenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

void SceneManager::RemoveEntity(Entity* entity, Scene* scene)
{
	// Unload the entity
	entity->OnUnloadScene();
	entity->m_LoadedInNrScenes--;

	// Remove from the scene
	scene->RemoveEntity(entity->GetName());

	// TODO: Temp fix, re init the scene
	SetScenes(m_ActiveScenes.size(), m_ActiveScenes.data());
}

void SceneManager::AddEntity(Entity* entity, Scene* scene)
{
	// Load the enity
	entity->OnLoadScene();
	entity->m_LoadedInNrScenes++;

	// Add it to the scene
	scene->AddEntityFromOther(entity);

	// TODO: Temp fix, re init the scene
	SetScenes(m_ActiveScenes.size(), m_ActiveScenes.data());
}

void SceneManager::SetScenes(unsigned int numScenes, Scene** scenes)
{
	ResetScene();

	std::vector<Scene*> lastActiveScenes = m_ActiveScenes;

	// Set the active scenes
	m_ActiveScenes.clear();
	
	for (unsigned int i = 0; i < numScenes; i++)
	{
		// Load scene if not loaded
		if (m_LoadedScenes.count(scenes[i]) < 1)
		{
			// load the scene if not loaded
 			LoadScene(scenes[i]);
		}

		// init the active scenes
		std::map<std::string, Entity*> entities = *(scenes[i]->GetEntities());
		for (auto const& [entityName, entity] : entities)
		{
			// Only init once per scene (in case a entity is in both scenes)
			if (m_IsEntityInited.count(entity) == 0)
			{
				entity->OnInitScene();
				m_IsEntityInited[entity] = true;
			}
		}

		m_ActiveScenes.push_back(scenes[i]);
	}

	Renderer* renderer = &Renderer::GetInstance();
	renderer->prepareScenes(&m_ActiveScenes);

	for (Scene* scene : m_ActiveScenes)
	{
		if (scene->GetMainCamera() == nullptr)
		{
			scene->SetPrimaryCamera(renderer->m_pScenePrimaryCamera);
		}
	}

	



	executeCopyOnDemand();
	return;
}

void SceneManager::LoadScene(Scene* scene)
{
	// Don't load if already loaded
	if (m_LoadedScenes.count(scene) >= 1)
	{
		return;
	}

	// Load the scene
	// Makes sure the model gets created/sent to gpu
	for (auto const& [name, entity] : *scene->GetEntities())
	{
		// Load only first time entity is referenced in a scene
		if (entity->m_LoadedInNrScenes == 0)
		{
			entity->OnLoadScene();
		}
		entity->m_LoadedInNrScenes++;
	}

	m_LoadedScenes.insert(scene);
}

void SceneManager::UnloadScene(Scene* scene)
{
	// Unload the scene
	for (auto const& [name, entity] : *scene->GetEntities())
	{
		// don't unload entities used by other scenes
		if (entity->m_LoadedInNrScenes == 1)
		{
			entity->OnUnloadScene();
		}
		entity->m_LoadedInNrScenes--;
	}

	m_LoadedScenes.erase(scene);
}

void SceneManager::ResetScene()
{
	// Reset isEntityInited
	m_IsEntityInited.clear();

	/* ------------------------- GPU -------------------------*/
	
	Renderer::GetInstance().OnResetScene();

	/* ------------------------- GPU -------------------------*/

	/* ------------------------- PHYSICS -------------------------*/

	Physics::GetInstance().OnResetScene();

	/* ------------------------- PHYSICS -------------------------*/

	/* ------------------------- Audio -------------------------*/

	/* ------------------------- Audio -------------------------*/
}

bool SceneManager::sceneExists(std::string sceneName) const
{
    for (auto pair : m_pScenes)
    {
        // A Scene with this m_Name already exists
        if (pair.first == sceneName)
        {
            return true;
        }
    }

    return false;
}

void SceneManager::executeCopyOnDemand()
{
	Renderer* renderer = &Renderer::GetInstance();
	renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->SetCommandInterfaceIndex(0);
	renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Execute();
	renderer->m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(1, &renderer->m_CopyOnDemandCmdList[0]);
	renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Clear();
	renderer->waitForCopyOnDemand();
}

