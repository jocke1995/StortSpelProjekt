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
	this->renderer = r;
}

SceneManager::~SceneManager()
{
    for (auto pair : this->scenes)
    {
        delete pair.second;
    }
    this->scenes.clear();
}

Scene* SceneManager::CreateScene(std::string sceneName)
{
    if (this->SceneExists(sceneName))
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "A scene with the name: \'%s\' already exists.\n", sceneName.c_str());
        return nullptr;
    }

    // Create Scene and return it
    this->scenes[sceneName] = new Scene(sceneName);
    return this->scenes[sceneName];
}

Scene* SceneManager::GetScene(std::string sceneName) const
{
    if (this->SceneExists(sceneName))
    {
        return this->scenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

bool SceneManager::SceneExists(std::string sceneName) const
{
    for (auto pair : this->scenes)
    {
        // A Scene with this name already exists
        if (pair.first == sceneName)
        {
            return true;
        }
    }

    return false;
}

// Handle and structure of the components in the scene
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
		for (int i = 0; i < this->renderer->m_RenderComponents.size(); i++)
		{
			Entity* parent = this->renderer->m_RenderComponents[i].first->GetParent();
			if (parent == entity)
			{
				this->renderer->m_RenderComponents.erase(this->renderer->m_RenderComponents.begin() + i);
				this->renderer->setRenderTasksRenderComponents();

				// Remove from CopyPerFrame
				component::MeshComponent* mc = parent->GetComponent<component::MeshComponent>();
				for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
				{
					const ConstantBufferView* cbv = mc->GetMesh(i)->GetMaterial()->GetConstantBufferView();
					CopyPerFrameTask * cpft = nullptr;
					cpft = static_cast<CopyPerFrameTask*>(this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
					cpft->ClearSpecific(cbv->GetUploadResource());
				}	
				break;
			}
		}

		// Check if the entity got any light components.
		// Remove them and update both cpu/gpu resources
		component::DirectionalLightComponent* dlc;
		component::PointLightComponent* plc;
		component::SpotLightComponent* slc;

		for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
		{
			LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
			unsigned int j = 0;

			for (auto& tuple : this->renderer->m_Lights[type])
			{
				Light* light = std::get<0>(tuple);
				Entity* parent = nullptr;

				// Find parent
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
					// Free memory so other entities can use it
					ConstantBufferView* cbv = std::get<1>(tuple);
					ShadowInfo* si = std::get<2>(tuple);
					this->renderer->m_pViewPool->ClearSpecificLight(type, cbv, si);
					
					// Remove from CopyPerFrame
					CopyPerFrameTask* cpft = nullptr;
					cpft = static_cast<CopyPerFrameTask*>(this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
					cpft->ClearSpecific(cbv->GetUploadResource());

					// Finally remove from renderer
					this->renderer->m_Lights[type].erase(this->renderer->m_Lights[type].begin() + j);

					// Update cbPerScene
					this->renderer->prepareCBPerScene();
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
				this->renderer->m_pWireFrameTask->ClearSpecific(bbc);
			}

			// Stop picking this boundingBox
			unsigned int i = 0;
			for (auto& bbcToBePicked : this->renderer->m_BoundingBoxesToBePicked)
			{
				if (bbcToBePicked == bbc)
				{
					this->renderer->m_BoundingBoxesToBePicked.erase(this->renderer->m_BoundingBoxesToBePicked.begin() + i);
					break;
				}
				i++;
			}
		}
		return;
	}
#pragma endregion RemoveEntity

#pragma region AddEntity
	// Only add the entities that actually should be drawn
	component::MeshComponent* mc = entity->GetComponent<component::MeshComponent>();
	if (mc != nullptr)
	{
		component::TransformComponent* tc = entity->GetComponent<component::TransformComponent>();
		if (tc != nullptr)
		{
			Mesh* mesh = mc->GetMesh(0);
			AssetLoader* al = AssetLoader::Get();
			std::wstring modelPath = to_wstring(mesh->GetPath());
			bool isModelOnGpu = al->loadedModels[modelPath].first;

			// If the model isn't on GPU, it will be uploaded below
			if (isModelOnGpu == false)
			{
				al->loadedModels[modelPath].first = true;
			}

			// Submit Material and Mesh/texture data to GPU if they haven't already been uploaded
			for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
			{
				mesh = mc->GetMesh(i);

				// Add material cbv
				ConstantBufferView* cbv = this->renderer->m_pViewPool->GetFreeCBV(sizeof(MaterialAttributes), L"Material" + i);
				mesh->GetMaterial()->SetCBV(cbv);

				// Submit to the list which gets updated to the gpu each frame
				CopyPerFrameTask* cpft = static_cast<CopyPerFrameTask*>(this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
				const void* data = static_cast<const void*>(mesh->GetMaterial()->GetMaterialAttributes());
				cpft->Submit(&std::make_tuple(cbv->GetUploadResource(), cbv->GetCBVResource(), data));

				// Submit mesh & texture Data to GPU if the data isn't already uploaded
				if (isModelOnGpu == false)
				{
					CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);

					// Vertices
					data = static_cast<const void*>(mesh->m_Vertices.data());
					Resource* uploadR = mesh->m_pUploadResourceVertices;
					Resource* defaultR = mesh->m_pDefaultResourceVertices;
					codt->Submit(&std::make_tuple(uploadR, defaultR, data));

					// inidices
					data = static_cast<const void*>(mesh->m_Indices.data());
					uploadR = mesh->m_pUploadResourceIndices;
					defaultR = mesh->m_pDefaultResourceIndices;
					codt->Submit(&std::make_tuple(uploadR, defaultR, data));

					// Textures
					for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
					{
						TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
						Texture* texture = mesh->GetMaterial()->GetTexture(type);
						
						// Check if the texture is on GPU before submitting to be uploaded
						if (al->loadedTextures[texture->m_FilePath].first == false)
						{
							codt->SubmitTexture(texture);
							al->loadedTextures[texture->m_FilePath].first = true;
						}
					}
				}
			}

			// Finally store the object in renderer so it will be drawn
			this->renderer->m_RenderComponents.push_back(std::make_pair(mc, tc));
		}
	}

	component::DirectionalLightComponent* dlc = entity->GetComponent<component::DirectionalLightComponent>();
	if (dlc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"DirectionalLight_DefaultResource";
		ConstantBufferView* cbd = this->renderer->m_pViewPool->GetFreeCBV(sizeof(DirectionalLight), resourceName);

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
			si = this->renderer->m_pViewPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
			static_cast<DirectionalLight*>(dlc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(this->renderer->m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(dlc, si));
		}

		// Save in renderer
		this->renderer->m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].push_back(std::make_tuple(dlc, cbd, si));
	}

	// Currently no shadows are implemented for pointLights
	component::PointLightComponent* plc = entity->GetComponent<component::PointLightComponent>();
	if (plc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"PointLight_DefaultResource";
		ConstantBufferView* cbd = this->renderer->m_pViewPool->GetFreeCBV(sizeof(PointLight), resourceName);

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;

		// Save in renderer
		this->renderer->m_Lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(plc, cbd, si));
	}

	component::SpotLightComponent* slc = entity->GetComponent<component::SpotLightComponent>();
	if (slc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"SpotLight_DefaultResource";
		ConstantBufferView* cbd = this->renderer->m_pViewPool->GetFreeCBV(sizeof(SpotLight), resourceName);

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
			si = this->renderer->m_pViewPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
			static_cast<SpotLight*>(slc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(this->renderer->m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(slc, si));
		}
		// Save in renderer
		this->renderer->m_Lights[LIGHT_TYPE::SPOT_LIGHT].push_back(std::make_tuple(slc, cbd, si));
	}

	component::CameraComponent* cc = entity->GetComponent<component::CameraComponent>();
	if (cc != nullptr)
	{
		if (cc->IsPrimary() == true)
		{
			this->renderer->m_pScenePrimaryCamera = cc->GetCamera();
		}
	}

	component::BoundingBoxComponent* bbc = entity->GetComponent<component::BoundingBoxComponent>();
	if (bbc != nullptr)
	{
		// Add it to task so it can be drawn
		if (DRAWBOUNDINGBOX == true)
		{
			Mesh* m = BoundingBoxPool::Get()->CreateBoundingBoxMesh(bbc->GetPathOfModel());
			if (m == nullptr)
			{
				Log::PrintSeverity(Log::Severity::WARNING, "Forgot to initialize BoundingBoxComponent on Entity: %s\n", bbc->GetParent()->GetName().c_str());
				return;
			}

			// Submit to GPU
			CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
			// Vertices
			const void* data = static_cast<const void*>(m->m_Vertices.data());
			Resource* uploadR = m->m_pUploadResourceVertices;
			Resource* defaultR = m->m_pDefaultResourceVertices;
			codt->Submit(&std::tuple(uploadR, defaultR, data));

			// inidices
			data = static_cast<const void*>(m->m_Indices.data());
			uploadR = m->m_pUploadResourceIndices;
			defaultR = m->m_pDefaultResourceIndices;
			codt->Submit(&std::tuple(uploadR, defaultR, data));

			bbc->SetMesh(m);

			this->renderer->m_pWireFrameTask->AddObjectToDraw(bbc);
		}

		// Add to vector so the mouse picker can check for intersections
		if (bbc->CanBePicked() == true)
		{
			this->renderer->m_BoundingBoxesToBePicked.push_back(bbc);
		}
	}
#pragma endregion AddEntity
}

void SceneManager::ExecuteCopyOnDemand()
{
	this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->SetCommandInterfaceIndex(0);
	this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Execute();
	this->renderer->m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(1, &this->renderer->m_CopyOnDemandCmdList[0]);
	this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Clear();
	this->renderer->waitForCopyOnDemand();
}

void SceneManager::ResetScene()
{
	// Reset
	this->renderer->m_RenderComponents.clear();
	for (auto& light : this->renderer->m_Lights)
	{
		light.second.clear();
	}
	this->renderer->m_pViewPool->ClearAll();
	this->renderer->m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	this->renderer->m_pScenePrimaryCamera = nullptr;
	this->renderer->m_pWireFrameTask->Clear();
	this->renderer->m_BoundingBoxesToBePicked.clear();
}
