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

SceneManager::SceneManager(Renderer* r)
{
	m_pRenderer = r;
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
    if (SceneExists(sceneName))
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
    if (SceneExists(sceneName))
    {
        return m_pScenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

bool SceneManager::SceneExists(std::string sceneName) const
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

// Handle and structure of the m_Components in the scene
void SceneManager::HandleSceneComponents(Scene* scene)
{
	std::map<std::string, Entity*> entities = *scene->GetEntities();
	for (auto const& [entityName, entity] : entities)
	{
		ManageComponent(entity, false);
	}
}

void SceneManager::ManageComponent(Entity* entity, bool remove)
{
#pragma region RemoveEntity
	if (remove == true)
	{
		// Check if the entity is a renderComponent
		for (int i = 0; i < m_pRenderer->renderComponents.size(); i++)
		{
			Entity* parent = m_pRenderer->renderComponents[i].first->GetParent();
			if (parent == entity)
			{
				m_pRenderer->renderComponents.erase(m_pRenderer->renderComponents.begin() + i);
				m_pRenderer->SetRenderTasksRenderComponents();

				// Remove from CopyPerFrame
				component::MeshComponent* mc = parent->GetComponent<component::MeshComponent>();
				for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
				{
					const ConstantBufferView* cbv = mc->GetMesh(i)->GetMaterial()->GetConstantBufferView();
					CopyPerFrameTask * cpft = nullptr;
					cpft = static_cast<CopyPerFrameTask*>(m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
					cpft->ClearSpecific(cbv->GetUploadResource());
				}	
				break;
			}
		}

		// Check if the entity got any light m_Components.
		// Remove them and update both cpu/gpu m_Resources
		component::DirectionalLightComponent* dlc;
		component::PointLightComponent* plc;
		component::SpotLightComponent* slc;

		for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
		{
			LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
			unsigned int j = 0;

			for (auto& tuple : m_pRenderer->lights[type])
			{
				Light* light = std::get<0>(tuple);
				Entity* parent = nullptr;

				// Find m_pParent
				switch (type)
				{
				case LIGHT_TYPE::DIRECTIONAL_LIGHT:
					dlc = static_cast<component::DirectionalLightComponent*>(light);
					parent = dlc->GetParent();
					break;
				case LIGHT_TYPE::POINT_LIGHT:
					plc = static_cast<component::PointLightComponent*>(light);
					parent = plc->GetParent();
					break;
				case LIGHT_TYPE::SPOT_LIGHT:
					slc = static_cast<component::SpotLightComponent*>(light);
					parent = slc->GetParent();
					break;
				}

				// Remove light if it matches the entity
				if (parent == entity)
				{
					// Free memory so other m_Entities can use it
					ConstantBufferView* cbv = std::get<1>(tuple);
					ShadowInfo* si = std::get<2>(tuple);
					m_pRenderer->viewPool->ClearSpecificLight(type, cbv, si);
					
					// Remove from CopyPerFrame
					CopyPerFrameTask* cpft = nullptr;
					cpft = static_cast<CopyPerFrameTask*>(m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
					cpft->ClearSpecific(cbv->GetUploadResource());

					// Finally remove from m_pRenderer
					m_pRenderer->lights[type].erase(m_pRenderer->lights[type].begin() + j);

					// Update cbPerScene
					m_pRenderer->PrepareCBPerScene();
					break;
				}
				j++;
			}
		}

		// Check if the entity got a boundingbox component.
		component::BoundingBoxComponent* bbc = entity->GetComponent<component::BoundingBoxComponent>();
		if (bbc->GetParent() == entity)
		{
			// Stop drawing the wireFrame
			if (DRAWBOUNDINGBOX == true)
			{
				m_pRenderer->wireFrameTask->ClearSpecific(bbc);
			}

			// Stop picking this boundingBox
			unsigned int i = 0;
			for (auto& bbcToBePicked : m_pRenderer->boundingBoxesToBePicked)
			{
				if (bbcToBePicked == bbc)
				{
					m_pRenderer->boundingBoxesToBePicked.erase(m_pRenderer->boundingBoxesToBePicked.begin() + i);
					break;
				}
				i++;
			}
		}
		return;
	}
#pragma endregion RemoveEntity

#pragma region AddEntity
	// Only add the m_Entities that actually should be drawn
	component::MeshComponent* mc = entity->GetComponent<component::MeshComponent>();
	if (mc != nullptr)
	{
		component::TransformComponent* tc = entity->GetComponent<component::TransformComponent>();
		if (tc != nullptr)
		{
			Mesh* mesh = mc->GetMesh(0);
			AssetLoader* al = AssetLoader::Get();
			std::wstring modelPath = to_wstring(mesh->GetPath());
			bool isModelOnGpu = al->m_LoadedModels[modelPath].first;

			// If the model isn't on GPU, it will be uploaded below
			if (isModelOnGpu == false)
			{
				al->m_LoadedModels[modelPath].first = true;
			}

			// Submit Material and Mesh/texture data to GPU if they haven't already been uploaded
			for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
			{
				mesh = mc->GetMesh(i);

				// Add material cbv
				ConstantBufferView* cbv = m_pRenderer->viewPool->GetFreeCBV(sizeof(MaterialAttributes), L"Material" + i);
				mesh->GetMaterial()->SetCBV(cbv);

				// Submit to the list which gets updated to the gpu each frame
				CopyPerFrameTask* cpft = static_cast<CopyPerFrameTask*>(m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
				const void* data = static_cast<const void*>(mesh->GetMaterial()->GetMaterialAttributes());
				cpft->Submit(&std::make_tuple(cbv->GetUploadResource(), cbv->GetCBVResource(), data));

				// Submit m_pMesh & texture Data to GPU if the data isn't already uploaded
				if (isModelOnGpu == false)
				{
					CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);

					// Vertices
					data = static_cast<const void*>(mesh->vertices.data());
					Resource* uploadR = mesh->uploadResourceVertices;
					Resource* defaultR = mesh->defaultResourceVertices;
					codt->Submit(&std::make_tuple(uploadR, defaultR, data));

					// inidices
					data = static_cast<const void*>(mesh->indices.data());
					uploadR = mesh->uploadResourceIndices;
					defaultR = mesh->defaultResourceIndices;
					codt->Submit(&std::make_tuple(uploadR, defaultR, data));

					// Textures
					for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
					{
						TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
						Texture* texture = mesh->GetMaterial()->GetTexture(type);
						
						// Check if the texture is on GPU before submitting to be uploaded
						if (al->m_LoadedTextures[texture->filePath].first == false)
						{
							codt->SubmitTexture(texture);
							al->m_LoadedTextures[texture->filePath].first = true;
						}
					}
				}
			}

			// Finally store the object in m_pRenderer so it will be drawn
			m_pRenderer->renderComponents.push_back(std::make_pair(mc, tc));
		}
	}

	component::DirectionalLightComponent* dlc = entity->GetComponent<component::DirectionalLightComponent>();
	if (dlc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"DirectionalLight_DefaultResource";
		ConstantBufferView* cbd = m_pRenderer->viewPool->GetFreeCBV(sizeof(DirectionalLight), resourceName);

		// Check if the light is to cast shadows
		SHADOW_RESOLUTION resolution = SHADOW_RESOLUTION::UNDEFINED;

		if (dlc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::LOW;
		}
		else if (dlc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::MEDIUM;
		}
		else if (dlc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::HIGH;
		}
		else if (dlc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::ULTRA;
		}

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;
		if (resolution != SHADOW_RESOLUTION::UNDEFINED)
		{
			si = m_pRenderer->viewPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
			static_cast<DirectionalLight*>(dlc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_pRenderer->renderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(dlc, si));
		}

		// Save in m_pRenderer
		m_pRenderer->lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].push_back(std::make_tuple(dlc, cbd, si));
	}

	// Currently no shadows are implemented for pointLights
	component::PointLightComponent* plc = entity->GetComponent<component::PointLightComponent>();
	if (plc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"PointLight_DefaultResource";
		ConstantBufferView* cbd = m_pRenderer->viewPool->GetFreeCBV(sizeof(PointLight), resourceName);

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;

		// Save in m_pRenderer
		m_pRenderer->lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(plc, cbd, si));
	}

	component::SpotLightComponent* slc = entity->GetComponent<component::SpotLightComponent>();
	if (slc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"SpotLight_DefaultResource";
		ConstantBufferView* cbd = m_pRenderer->viewPool->GetFreeCBV(sizeof(SpotLight), resourceName);

		// Check if the light is to cast shadows
		SHADOW_RESOLUTION resolution = SHADOW_RESOLUTION::UNDEFINED;

		if (slc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::LOW;
		}
		else if (slc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::MEDIUM;
		}
		else if (slc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::HIGH;
		}
		else if (slc->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
		{
			resolution = SHADOW_RESOLUTION::ULTRA;
		}

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;
		if (resolution != SHADOW_RESOLUTION::UNDEFINED)
		{
			si = m_pRenderer->viewPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
			static_cast<SpotLight*>(slc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_pRenderer->renderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(slc, si));
		}
		// Save in m_pRenderer
		m_pRenderer->lights[LIGHT_TYPE::SPOT_LIGHT].push_back(std::make_tuple(slc, cbd, si));
	}

	component::CameraComponent* cc = entity->GetComponent<component::CameraComponent>();
	if (cc != nullptr)
	{
		if (cc->IsPrimary() == true)
		{
			m_pRenderer->ScenePrimaryCamera = cc->GetCamera();
		}
	}

	component::BoundingBoxComponent* bbc = entity->GetComponent<component::BoundingBoxComponent>();
	if (bbc != nullptr)
	{
		// Add it to m_pTask so it can be drawn
		if (DRAWBOUNDINGBOX == true)
		{
			Mesh* m = BoundingBoxPool::Get()->CreateBoundingBoxMesh(bbc->GetPathOfModel());
			if (m == nullptr)
			{
				Log::PrintSeverity(Log::Severity::WARNING, "Forgot to initialize BoundingBoxComponent on Entity: %s\n", bbc->GetParent()->GetName().c_str());
				return;
			}

			// Submit to GPU
			CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
			// Vertices
			const void* data = static_cast<const void*>(m->vertices.data());
			Resource* uploadR = m->uploadResourceVertices;
			Resource* defaultR = m->defaultResourceVertices;
			codt->Submit(&std::tuple(uploadR, defaultR, data));

			// inidices
			data = static_cast<const void*>(m->indices.data());
			uploadR = m->uploadResourceIndices;
			defaultR = m->defaultResourceIndices;
			codt->Submit(&std::tuple(uploadR, defaultR, data));

			bbc->SetMesh(m);

			m_pRenderer->wireFrameTask->AddObjectToDraw(bbc);
		}

		// Add to vector so the mouse picker can check for intersections
		if (bbc->CanBePicked() == true)
		{
			m_pRenderer->boundingBoxesToBePicked.push_back(bbc);
		}
	}
#pragma endregion AddEntity
}

void SceneManager::ExecuteCopyOnDemand()
{
	m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->SetCommandInterfaceIndex(0);
	m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Execute();
	m_pRenderer->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(1, &m_pRenderer->m_CopyOnDemandCmdList[0]);
	m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Clear();
	m_pRenderer->WaitForCopyOnDemand();
}

void SceneManager::ResetScene()
{
	// Reset
	m_pRenderer->renderComponents.clear();
	for (auto& light : m_pRenderer->lights)
	{
		light.second.clear();
	}
	m_pRenderer->viewPool->ClearAll();
	m_pRenderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	m_pRenderer->ScenePrimaryCamera = nullptr;
	m_pRenderer->wireFrameTask->Clear();
	m_pRenderer->boundingBoxesToBePicked.clear();
}
