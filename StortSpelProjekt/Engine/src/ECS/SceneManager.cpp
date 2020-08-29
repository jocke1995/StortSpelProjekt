#include "SceneManager.h"

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
		for (int i = 0; i < this->renderer->renderComponents.size(); i++)
		{
			Entity* parent = this->renderer->renderComponents[i].first->GetParent();
			if (parent == entity)
			{
				this->renderer->renderComponents.erase(this->renderer->renderComponents.begin() + i);
				this->renderer->SetRenderTasksRenderComponents();
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

			for (auto& tuple : this->renderer->lights[type])
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
					this->renderer->cbvPool->ClearSpecificLight(type, cbv, si);
					
					// Remove from CopyPerFrame
					CopyPerFrameTask* cpft = nullptr;
					cpft = static_cast<CopyPerFrameTask*>(this->renderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
					cpft->ClearSpecific(cbv);

					// Finally remove from renderer
					this->renderer->lights[type].erase(this->renderer->lights[type].begin() + j);

					// Update cbPerScene
					this->renderer->PrepareCBPerScene();
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
				this->renderer->wireFrameTask->ClearSpecific(bbc);
			}

			// Stop picking this boundingBox
			unsigned int i = 0;
			for (auto& bbcToBePicked : this->renderer->boundingBoxesToBePicked)
			{
				if (bbcToBePicked == bbc)
				{
					this->renderer->boundingBoxesToBePicked.erase(this->renderer->boundingBoxesToBePicked.begin() + i);
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
			this->renderer->renderComponents.push_back(std::make_pair(mc, tc));

			// Send the Textures to GPU here later, so that textures aren't in memory if they aren't used
			// or submit index to a queue and then submit all textures together later..
		}
	}

	component::DirectionalLightComponent* dlc = entity->GetComponent<component::DirectionalLightComponent>();
	if (dlc != nullptr)
	{
		// Assign CBV from the lightPool
		unsigned int entrySize = (sizeof(DirectionalLight) + 255) & ~255;	// align to 255-byte boundary
		std::wstring resourceName = L"DirectionalLight_DefaultResource";
		ConstantBufferView* cbd = this->renderer->cbvPool->GetFreeCBV(entrySize, resourceName);

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

			si = this->renderer->cbvPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
			static_cast<DirectionalLight*>(dlc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(this->renderer->renderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(dlc, si));
		}

		// Save in renderer
		this->renderer->lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].push_back(std::make_tuple(dlc, cbd, si));
	}

	// Currently no shadows are implemented for pointLights
	component::PointLightComponent* plc = entity->GetComponent<component::PointLightComponent>();
	if (plc != nullptr)
	{
		// Assign CBV from the lightPool
		unsigned int entrySize = (sizeof(PointLight) + 255) & ~255;	// align to 255-byte boundary
		std::wstring resourceName = L"PointLight_DefaultResource";
		ConstantBufferView* cbd = this->renderer->cbvPool->GetFreeCBV(entrySize, resourceName);

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;

		// Save in renderer
		this->renderer->lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(plc, cbd, si));
	}

	component::SpotLightComponent* slc = entity->GetComponent<component::SpotLightComponent>();
	if (slc != nullptr)
	{
		// Assign CBV from the lightPool
		unsigned int entrySize = (sizeof(SpotLight) + 255) & ~255;	// align to 255-byte boundary
		std::wstring resourceName = L"SpotLight_DefaultResource";
		ConstantBufferView* cbd = this->renderer->cbvPool->GetFreeCBV(entrySize, resourceName);

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
			si = this->renderer->cbvPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
			static_cast<SpotLight*>(slc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(this->renderer->renderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(slc, si));
		}
		// Save in renderer
		this->renderer->lights[LIGHT_TYPE::SPOT_LIGHT].push_back(std::make_tuple(slc, cbd, si));
	}

	component::CameraComponent* cc = entity->GetComponent<component::CameraComponent>();
	if (cc != nullptr)
	{
		if (cc->IsPrimary() == true)
		{
			this->renderer->ScenePrimaryCamera = cc->GetCamera();
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

			// Upload to Default heap
			m->UploadToDefault(
				this->renderer->device5,
				this->renderer->tempCommandInterface,
				this->renderer->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
			this->renderer->WaitForGpu();

			bbc->SetMesh(m);

			this->renderer->wireFrameTask->AddObjectToDraw(bbc);
		}

		// Add to vector so the mouse picker can check for intersections
		if (bbc->CanBePicked() == true)
		{
			this->renderer->boundingBoxesToBePicked.push_back(bbc);
		}
	}
#pragma endregion AddEntity
}

void SceneManager::ResetScene()
{
	// Reset
	this->renderer->renderComponents.clear();
	for (auto& light : this->renderer->lights)
	{
		light.second.clear();
	}
	this->renderer->cbvPool->Clear();
	this->renderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	this->renderer->ScenePrimaryCamera = nullptr;
	this->renderer->wireFrameTask->Clear();
	this->renderer->boundingBoxesToBePicked.clear();
}
