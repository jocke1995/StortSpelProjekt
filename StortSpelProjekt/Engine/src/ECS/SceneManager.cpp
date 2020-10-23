#include "stdafx.h"
#include "SceneManager.h"

#include "../Misc/AssetLoader.h"
#include "../Events/EventBus.h"

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

	EventBus::GetInstance().Subscribe(this, &SceneManager::changeSceneNextFrame);
}

SceneManager::~SceneManager()
{
	for (auto pair : m_Scenes)
	{
		delete pair.second;
	}

    m_Scenes.clear();
	EventBus::GetInstance().Unsubscribe(this, &SceneManager::changeSceneNextFrame);
}

void SceneManager::Update(double dt)
{
	// Update scenes
	for (auto scene : m_ActiveScenes)
	{
		scene->Update(this, dt);
	}
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

void SceneManager::ChangeSceneIfTeleported()
{
	if (m_ChangeSceneNextFrame == true)
	{
		// Set new scene
		if (m_ActiveScenes[0]->GetName() != m_SceneToChangeToWhenTeleported)
		{
			Scene* scene = m_Scenes[m_SceneToChangeToWhenTeleported];

			// Change the player back to its original position
			Transform* trans = scene->GetEntity("player")->GetComponent<component::TransformComponent>()->GetTransform();
			trans->SetPosition({ scene->m_OriginalPosition.x, scene->m_OriginalPosition.y, scene->m_OriginalPosition.z });
			SetScenes(1, &scene);
			m_ChangeSceneNextFrame = false;
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

void SceneManager::SetScenes(unsigned int numScenes, Scene** scenes)
{
	ResetScene();

	// Set the active scenes
	m_ActiveScenes.clear();
	
	for (unsigned int i = 0; i < numScenes; i++)
	{
		// init the active scenes
		std::map<std::string, Entity*> entities = *(scenes[i]->GetEntities());
		for (auto const& [entityName, entity] : entities)
		{
			entity->OnInitScene();
		}

		m_ActiveScenes.push_back(scenes[i]);
	}
	// TODO: Currently only works for 1 scene. (1 scene active at a given time)
	Physics::GetInstance().SetCollisionEntities(scenes[0]->GetCollisionEntities());

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

void SceneManager::changeSceneNextFrame(SceneChange* sceneChangeEvent)
{
	m_SceneToChangeToWhenTeleported = sceneChangeEvent->m_NewSceneName;
	m_ChangeSceneNextFrame = true;
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