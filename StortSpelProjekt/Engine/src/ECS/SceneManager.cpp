#include "stdafx.h"
#include "SceneManager.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"
#include "../Physics/Physics.h"
#include "../Events/EventBus.h"

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
	EventBus::GetInstance().Subscribe(this, &SceneManager::onEntityRemove);
	EventBus::GetInstance().Subscribe(this, &SceneManager::changeSceneNextFrame);
}

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
}

SceneManager::~SceneManager()
{
}

void SceneManager::deleteSceneManager()
{
	EventBus::GetInstance().Unsubscribe(this, &SceneManager::onEntityRemove);
	EventBus::GetInstance().Unsubscribe(this, &SceneManager::changeSceneNextFrame);

	for (auto pair : m_Scenes)
	{
		delete pair.second;
	}

	m_Scenes.clear();
}

void SceneManager::Update(double dt)
{
	// Update scene
	m_pActiveScene->Update(this, dt);

	unsigned int removeSize = m_ToRemove.size() - 1;
	for (int i = removeSize; i >= 0; --i)
	{
		RemoveEntity(m_ToRemove[i].ent, m_ToRemove[i].scene);
	}
	m_ToRemove.clear();
}

void SceneManager::RenderUpdate(double dt)
{
	// Update scenes (Render)
	m_pActiveScene->RenderUpdate(this, dt);

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
    m_Scenes[sceneName] = new Scene(sceneName);
    return m_Scenes[sceneName];
}

Scene* SceneManager::GetActiveScene()
{
	return m_pActiveScene;
}

Scene* SceneManager::GetScene(std::string sceneName) const
{
    if (sceneExists(sceneName))
    {
        return m_Scenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

void SceneManager::ChangeScene()
{
	if (m_ChangeSceneNextFrame)
	{
		if (m_ActiveScenes[0]->GetName() != "gameOverScene")
		{
			// Reset old scene
			std::map<std::string, Entity*> entities = *m_pActiveScene->GetEntities();
			for (auto pair : entities)
			{
				for (Component* comp : *pair.second->GetAllComponents())
				{
					comp->Reset();
				}
			}

			Scene* scene = m_Scenes[m_SceneToChangeTo];

			// Reset new Scene
			entities = *scene->GetEntities();
			for (auto pair : entities)
			{
				for (Component* comp : *pair.second->GetAllComponents())
				{
					comp->Reset();
				}
			}

			// Change the player back to its original position
			SetScene(scene);
			m_ChangeSceneNextFrame = false;
		}
		else
		{
			SetScene(m_pGameOverScene);
			m_ChangeSceneNextFrameToDeathScene = false;

			Physics::GetInstance().OnResetScene();
			EventBus::GetInstance().UnsubscribeAll();
		}
	}
}

void SceneManager::RemoveEntity(Entity* entity, Scene* scene)
{
	entity->OnUnInitScene();

	// Remove from the scene
	scene->RemoveEntity(entity->GetName());
}

void SceneManager::AddEntity(Entity* entity, Scene* scene)
{
	entity->OnInitScene();
}

void SceneManager::SetGameOverScene(Scene* scene)
{
	if (scene != nullptr)
	{
		m_pGameOverScene = scene;
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "SetGameOverScene:: scene was nullptr");
	}
}

void SceneManager::SetScene(Scene* scene)
{
	if (scene == m_pActiveScene)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "SetScene on same scene %s\n", scene->GetName());
		return;
	}

	ResetScene();

	if (m_pActiveScene != nullptr)
	{
		std::map<std::string, Entity*> oldEntities = *m_pActiveScene->GetEntities();
		for (auto pair : oldEntities)
		{
			Entity* ent = pair.second;
			if (ent->IsEntityDynamic() == true)
			{
				m_pActiveScene->RemoveEntity(ent->GetName());
			}
		}

		for (auto const& [entityName, entity] : oldEntities)
		{
			entity->OnUnInitScene();
		}
	}

	// init the active scenes
	std::map<std::string, Entity*> entities = *scene->GetEntities();
	for (auto const& [entityName, entity] : entities)
	{
		entity->SetEntityState(false);
		entity->OnInitScene();
	}

	m_pActiveScene = scene;

	Physics::GetInstance().SetCollisionEntities(scene->GetCollisionEntities());

	Renderer* renderer = &Renderer::GetInstance();
	renderer->prepareScene(m_pActiveScene);

	if (m_pActiveScene->GetMainCamera() == nullptr)
	{
		m_pActiveScene->SetPrimaryCamera(renderer->m_pScenePrimaryCamera);
	}

	return;
}

void SceneManager::ResetScene()
{
	Renderer::GetInstance().waitForGPU();

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
    for (auto pair : m_Scenes)
    {
        // A Scene with this m_Name already exists
        if (pair.first == sceneName)
        {
            return true;
        }
    }

    return false;
}

void SceneManager::onEntityRemove(RemoveMe* evnt)
{
	m_ToRemove.push_back({ evnt->ent, m_pActiveScene });
}

void SceneManager::changeSceneNextFrame(SceneChange* sceneChangeEvent)
{
	m_SceneToChangeTo = sceneChangeEvent->m_NewSceneName;
	m_ChangeSceneNextFrame = true;
}
