#include "stdafx.h"
#include "SceneManager.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"

// Renderer
#include "../Renderer/CommandInterface.h"
#include "../Renderer/ShaderResourceView.h"
#include "../Renderer/ConstantBufferView.h"
#include "../Renderer/Material.h"
#include "../Renderer/ShadowInfo.h"
#include "../Renderer/ViewPool.h"
#include "../Renderer/Texture.h"
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

// Audio
#include "../AudioEngine/AudioEngine.h"

SceneManager::SceneManager(Renderer* r, AudioEngine* audioEngine)
{
	m_pRenderer = r;
	m_pAudioEngine = audioEngine;
}

SceneManager::~SceneManager()
{
    for (auto pair : m_pScenes)
    {
        delete pair.second;
    }
    m_pScenes.clear();
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
	// Add renderer component
	m_pRenderer->addComponents(entity);

	// Add sound component

	// Add game component

	// Add physic component

	executeCopyOnDemand();
}

void SceneManager::SetSceneToDraw(Scene* scene)
{
	resetScene();

	std::map<std::string, Entity*> entities = *scene->GetEntities();
	for (auto const& [entityName, entity] : entities)
	{
		// Add renderer component returns 0
		m_pRenderer->addComponents(entity);

		// Add sound component
		// load sound files that are to be used in scene
		m_pAudioEngine->LoadAudioFiles(entity);

		// Add game component

		// Add physic component

	}
	
	m_pRenderer->prepareScene(scene);

	executeCopyOnDemand();
	return;
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

void SceneManager::resetScene()
{
	// Reset
	m_pRenderer->m_RenderComponents.clear();
	for (auto& light : m_pRenderer->m_Lights)
	{
		light.second.clear();
	}
	m_pRenderer->m_pViewPool->ClearAll();
	m_pRenderer->m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	static_cast<ShadowRenderTask*>(m_pRenderer->m_RenderTasks[RENDER_TASK_TYPE::SHADOW])->Clear();
	m_pRenderer->m_pScenePrimaryCamera = nullptr;
	m_pRenderer->m_pWireFrameTask->Clear();
	m_pRenderer->m_BoundingBoxesToBePicked.clear();
}