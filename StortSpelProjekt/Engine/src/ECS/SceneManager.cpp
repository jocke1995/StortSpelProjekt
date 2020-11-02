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
	m_ActiveScenes.reserve(1);
	// "Default scene"
	Scene* m_DefaultScene = new Scene("defaultScene");
	m_Scenes["defaultScene"] = m_DefaultScene;
	m_ActiveScenes.push_back(m_DefaultScene); 

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

void SceneManager::EraseSceneManager()
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
	// Update scenes
	for (auto scene : m_ActiveScenes)
	{
		scene->Update(this, dt);
	}

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
	for (auto scene : m_ActiveScenes)
	{
		scene->RenderUpdate(this, dt);
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
    m_Scenes[sceneName] = new Scene(sceneName);
    return m_Scenes[sceneName];
}

std::vector<Scene*>* SceneManager::GetActiveScenes()
{
	return &m_ActiveScenes;
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
		if (m_ActiveScenes[0]->GetName() == "ShopScene" || m_ActiveScenes[0]->GetName() == "GameScene")
		{
			// Reset old scene
			std::map<std::string, Entity*> entities = *m_ActiveScenes[0]->GetEntities();
			for (auto pair : entities)
			{
				for (Component* comp : *pair.second->GetAllComponents())
				{
					comp->Reset();
				}
			}

			Scene* scene = m_Scenes[m_SceneToChangeToWhenTeleported];

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
			SetScenes(scene);
			m_ChangeSceneNextFrame = false;
		}
		else if (m_ActiveScenes[0]->GetName() == "gameOverScene")
		{
			SetScenes(m_pGameOverScene);
			m_ChangeSceneNextFrameToDeathScene = false;

			Physics::GetInstance().OnResetScene();
			EventBus::GetInstance().UnsubscribeAll();
		}
	}
}

void SceneManager::RemoveEntity(Entity* entity, Scene* scene)
{
	entity->OnUnInitScene();

	// Some components need to be sent to the gpu each frame
	Renderer::GetInstance().SubmitUploadPerFrameData();
	Renderer::GetInstance().SubmitUploadPerSceneData();

	// Remove from the scene
	scene->RemoveEntity(entity->GetName());
}

void SceneManager::AddEntity(Entity* entity, Scene* scene)
{
	entity->OnInitScene();

	// Some components need to be sent to the gpu each frame
	Renderer::GetInstance().SubmitUploadPerFrameData();
	Renderer::GetInstance().SubmitUploadPerSceneData();
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

void SceneManager::SetScenes(Scene* scene)
{
	if (scene == m_ActiveScenes.at(0))
	{
		Log::PrintSeverity(Log::Severity::WARNING, "SetScene on same scene %s\n", scene->GetName());
		return;
	}

	ResetScene();

	Scene* oldScene = m_ActiveScenes.at(0);

	std::map<std::string, Entity*> oldEntities = *oldScene->GetEntities();
	for (auto pair : oldEntities)
	{
		Entity* ent = pair.second;
		if (ent->IsEntityDynamic() == true)
		{
			oldScene->RemoveEntity(ent->GetName());
		}
	}

	// Check if we should uninit
	if (oldScene != scene)
	{
		for (auto const& [entityName, entity] : oldEntities)
		{
			entity->OnUnInitScene();
		}
	}

	// Set the active scenes
	m_ActiveScenes.clear();

	// init the active scenes
	std::map<std::string, Entity*> entities = *(scene->GetEntities());
	for (auto const& [entityName, entity] : entities)
	{
		entity->SetEntityState(false);
		entity->OnInitScene();
	}

	m_ActiveScenes.push_back(scene);

	Physics::GetInstance().SetCollisionEntities(scene->GetCollisionEntities());

	Renderer* renderer = &Renderer::GetInstance();
	renderer->prepareScenes(&m_ActiveScenes);

	for (Scene* scene : m_ActiveScenes)
	{
		if (scene->GetMainCamera() == nullptr)
		{
			scene->SetPrimaryCamera(renderer->m_pScenePrimaryCamera);
		}
	}

	return;
}

// Archived code
//void SceneManager::SetScenes(unsigned int numScenes, Scene** scenes)
//{
//	// Only support for 1 scene currently.
//	numScenes = 1;
//
//	ResetScene();
//
//	// Remove dynamic entities from m_ActiveScenes
//	if (m_ActiveScenes.size() > 0)
//	{
//		Scene* activeScene = m_ActiveScenes[0];
//
//		std::map<std::string, Entity*> entities = *m_ActiveScenes[0]->GetEntities();
//		for (auto pair : entities)
//		{
//			Entity* ent = pair.second;
//			if (ent->IsEntityDynamic() == true)
//			{
//				activeScene->RemoveEntity(ent->GetName());
//			}
//		}
//	}
//
//	// Set the active scenes
//	m_ActiveScenes.clear();
//	
//	for (unsigned int i = 0; i < numScenes; i++)
//	{
//		// init the active scenes
//		std::map<std::string, Entity*> entities = *(scenes[i]->GetEntities());
//		for (auto const& [entityName, entity] : entities)
//		{
//			entity->SetEntityState(false);
//			entity->OnInitScene();
//		}
//
//		m_ActiveScenes.push_back(scenes[i]);
//	}
//
//	Physics::GetInstance().SetCollisionEntities(scenes[0]->GetCollisionEntities());
//
//	Renderer* renderer = &Renderer::GetInstance();
//	renderer->prepareScenes(&m_ActiveScenes);
//
//	for (Scene* scene : m_ActiveScenes)
//	{
//		if (scene->GetMainCamera() == nullptr)
//		{
//			scene->SetPrimaryCamera(renderer->m_pScenePrimaryCamera);
//		}
//	}
//
//	return;
//}

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
	m_ToRemove.push_back({ evnt->ent, m_ActiveScenes[0] });
}

void SceneManager::changeSceneNextFrame(SceneChange* sceneChangeEvent)
{
	m_SceneToChangeToWhenTeleported = sceneChangeEvent->m_NewSceneName;
	m_ChangeSceneNextFrame = true;
}
