#include "SceneHandler.h"

SceneHandler::SceneHandler(Renderer* r)
{
	this->renderer = r;
}

SceneHandler::~SceneHandler()
{
    for (auto pair : this->scenes)
    {
        delete pair.second;
    }
    this->scenes.clear();
}

Scene* SceneHandler::CreateScene(std::string sceneName)
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

Scene* SceneHandler::GetScene(std::string sceneName) const
{
    if (this->SceneExists(sceneName))
    {
        return this->scenes.at(sceneName);
    }
	
    Log::PrintSeverity(Log::Severity::CRITICAL, "No Scene with name: \'%s\' was found.\n", sceneName.c_str());
    return nullptr;
}

bool SceneHandler::SceneExists(std::string sceneName) const
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

// Handle and structure the components in the scene
void SceneHandler::HandleSceneComponents(Scene* scene)
{
	std::map<std::string, Entity*> entities = *scene->GetEntities();
	for (auto const& [entityName, entity] : entities)
	{
		ManageComponent(entity);
	}
}

void SceneHandler::ManageComponent(Entity* entity)
{
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
		ConstantBufferView* cbd = this->renderer->lightViewsPool->GetFreeConstantBufferView(LIGHT_TYPE::DIRECTIONAL_LIGHT);

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

			si = this->renderer->lightViewsPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
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
		// Assign resource from resourcePool
		ConstantBufferView* cbd = this->renderer->lightViewsPool->GetFreeConstantBufferView(LIGHT_TYPE::POINT_LIGHT);

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;

		// Save in renderer
		this->renderer->lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(plc, cbd, si));
	}

	component::SpotLightComponent* slc = entity->GetComponent<component::SpotLightComponent>();
	if (slc != nullptr)
	{
		// Assign resource from resourcePool
		ConstantBufferView* cbd = this->renderer->lightViewsPool->GetFreeConstantBufferView(LIGHT_TYPE::SPOT_LIGHT);

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
			si = this->renderer->lightViewsPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
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
				Log::PrintSeverity(Log::Severity::WARNING, "Forgot to initialize BoundingBoxComponent on Entity: %s\n", bbc->GetParentName().c_str());
				return;
			}

			// Upload to Default heap
			m->UploadToDefault(
				this->renderer->device5,
				this->renderer->tempCommandInterface,
				this->renderer->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
			this->renderer->WaitForGpu();

			bbc->SetMesh(m);

			this->renderer->wireFrameTask->AddObjectToDraw(&std::make_pair(m, bbc->GetTransform()));
		}

		// Add to vector so the mouse picker can check for intersections
		if (bbc->Pick() == true)
		{
			this->renderer->boundingBoxesToBePicked.push_back(bbc);
		}
	}
}

void SceneHandler::ResetScene()
{
	// Reset
	this->renderer->renderComponents.clear();
	for (auto& light : this->renderer->lights)
	{
		light.second.clear();
	}
	this->renderer->lightViewsPool->Clear();
	this->renderer->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	this->renderer->ScenePrimaryCamera = nullptr;
	this->renderer->wireFrameTask->Clear();
	this->renderer->boundingBoxesToBePicked.clear();
}
