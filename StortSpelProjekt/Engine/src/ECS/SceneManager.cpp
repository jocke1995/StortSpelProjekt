#include "stdafx.h"
#include "SceneManager.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"

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

SceneManager::SceneManager(Renderer* r)
{
	m_pRenderer = r;

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
	m_pRenderer->RenderUpdate(dt);
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

Scene* SceneManager::GetScene(std::string sceneName) const
{
    if (sceneExists(sceneName))
    {
        return m_pScenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

void SceneManager::RemoveEntity(Entity* entity)
{
	// Removing renderer component
	m_pRenderer->removeComponents(entity);

	// Remove sound component

	// Remove game component

	// Remove physic component

	executeCopyOnDemand();
}

void SceneManager::AddEntity(Entity* entity)
{
	// Add all components
	std::vector<Component*>* components = entity->GetAllComponents();
	for (int i = 0; i < components->size(); i++)
	{
		components->at(i)->InitScene();
	}

	executeCopyOnDemand();
}

void SceneManager::SetScene(unsigned int numScenes, Scene** scenes)
{
	ResetScene();

	// Set the active scenes
	m_ActiveScenes.clear();
	// Load all scenes if they are not loaded
	for (unsigned int i = 0; i < numScenes; i++)
	{
		if (m_LoadedScenes.count(scenes[i]) < 1)
		{
			// load the scene if not loaded
 			LoadScene(scenes[i]);
		}

		// init the active scenes
		std::map<std::string, Entity*> entities = *(scenes[i]->GetEntities());
		for (auto const& [entityName, entity] : entities)
		{
			// for each component in entity: call their implementation of InitScene(),
			// which calls their specific init function (render, audio, game, physics etc)
			std::vector<Component*>* components = entity->GetAllComponents();
			for (int i = 0; i < components->size(); i++)
			{
				components->at(i)->InitScene();
			}
		}

		m_ActiveScenes.push_back(scenes[i]);
	}
	
	m_pRenderer->prepareScenes(&m_ActiveScenes);
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
		// Load only first time entity is referenced in a c
		if (true)
		{
			component::ModelComponent* mc = entity->GetComponent<component::ModelComponent>();
			if (mc != nullptr)
			{
				// TODO: fix code with assetloader
				m_pRenderer->loadModel(AssetLoader::Get()->LoadModel(mc->GetModelPath()));
			}
		}
	}

	m_LoadedScenes.insert(scene);
}

void SceneManager::UnloadScene(Scene* scene)
{
	// GPU can't be running when removing resources
	m_pRenderer->waitForGPU();

	// Unload the scene
	for (auto const& [name, entity] : *scene->GetEntities())
	{
		// don't unload entities used by other scenes
		if (true)
		{
			// Unload the entity
  			component::ModelComponent* mc = entity->GetComponent<component::ModelComponent>();
			if (mc != nullptr)
			{
				// TODO: fix code with assetloader
				m_pRenderer->unloadModel(AssetLoader::Get()->LoadModel(mc->GetModelPath()));
			}
		}
	}

	m_LoadedScenes.erase(scene);
}

void SceneManager::ResetScene()
{

	/* ------------------------- GPU -------------------------*/
	
	m_pRenderer->m_RenderComponents.clear();
	for (auto& light : m_pRenderer->m_Lights)
	{
		light.second.clear();
	}
	m_pRenderer->m_pViewPool->ClearAll();
	m_pRenderer->m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	static_cast<ShadowRenderTask*>(m_pRenderer->m_RenderTasks[RENDER_TASK_TYPE::SHADOW])->Clear();
	m_pRenderer->m_pScenePrimaryCamera = nullptr;
	static_cast<WireframeRenderTask*>(m_pRenderer->m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME])->Clear();
	m_pRenderer->m_BoundingBoxesToBePicked.clear();
	m_pRenderer->m_TextComponents.clear();

	/* ------------------------- GPU -------------------------*/

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
	m_pRenderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->SetCommandInterfaceIndex(0);
	m_pRenderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Execute();
	m_pRenderer->m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(1, &m_pRenderer->m_CopyOnDemandCmdList[0]);
	m_pRenderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Clear();
	m_pRenderer->waitForCopyOnDemand();
}

