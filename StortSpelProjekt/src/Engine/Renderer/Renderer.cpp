#include "Renderer.h"

Renderer::Renderer()
{
	this->renderTasks.resize(RENDER_TASK_TYPE::NR_OF_RENDERTASKS);
	this->copyTasks.resize(COPY_TASK_TYPE::NR_OF_COPYTASKS);
	this->computeTasks.resize(COMPUTE_TASK_TYPE::NR_OF_COMPUTETASKS);
}

Renderer::~Renderer()
{
	Log::Print("----------------------------  Deleting Renderer  ----------------------------------\n");
	this->WaitForFrame(0);

	SAFE_RELEASE(&this->fenceFrame);
	if (!CloseHandle(this->eventHandle))
	{
		Log::Print("Failed To Close Handle... ErrorCode: %d\n", GetLastError());
	}

	SAFE_RELEASE(&this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
	SAFE_RELEASE(&this->commandQueues[COMMAND_INTERFACE_TYPE::COMPUTE_TYPE]);
	SAFE_RELEASE(&this->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]);

	delete this->rootSignature;
	delete this->swapChain;
	delete this->mainDSV;

	for (auto& pair : this->descriptorHeaps)
	{
		delete pair.second;
	}

	for (ComputeTask* computeTask : this->computeTasks)
		delete computeTask;

	for (CopyTask* copyTask : this->copyTasks)
		delete copyTask;

	for (RenderTask* renderTask : this->renderTasks)
		delete renderTask;
	delete this->wireFrameTask;

	SAFE_RELEASE(&this->device5);

	delete this->mousePicker;

	delete this->lightViewsPool;
	delete this->cbPerScene;
	delete this->cbPerFrame;
	delete this->cbPerFrameData;

	// temp
	delete this->tempCommandInterface;
}

void Renderer::InitD3D12(const HWND *hwnd, HINSTANCE hInstance, ThreadPool* threadPool)
{
	this->threadPool = threadPool;

	// Create Device
	if (!this->CreateDevice())
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Device\n");
	}

	// Create CommandQueues (copy, compute and direct)
	this->CreateCommandQueues();

	// Create DescriptorHeaps
	this->CreateDescriptorHeaps();

	// Fence for WaitForFrame();
	this->CreateFences();

	// Create SwapChain
	this->CreateSwapChain(hwnd);

	// Create Main DepthBuffer
	this->CreateMainDSV(hwnd);

	// Picking
	this->mousePicker = new MousePicker();
	
	// Create Rootsignature
	this->CreateRootSignature();

	// Init Assetloader
	AssetLoader::Get(this->device5, this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

	// Init BoundingBoxPool
	BoundingBoxPool::Get(this->device5, this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
	
	// Pool to handle GPU memory for the lights
	this->lightViewsPool = new LightViewsPool(
		this->device5,
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV]);

	// Allocate memory for cbPerScene
	unsigned int CB_PER_SCENE_SizeAligned = (sizeof(CB_PER_SCENE_STRUCT) + 255) & ~255;
	this->cbPerScene = new ConstantBufferView(
		this->device5, 
		CB_PER_SCENE_SizeAligned,
		L"CB_PER_SCENE_DEFAULT",
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetNextDescriptorHeapIndex(1),
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]
		);

	// Allocate memory for cbPerFrame
	unsigned int CB_PER_Frame_SizeAligned = (sizeof(CB_PER_FRAME_STRUCT) + 255) & ~255;
	this->cbPerFrame = new ConstantBufferView(
		this->device5,
		CB_PER_Frame_SizeAligned,
		L"CB_PER_FRAME_DEFAULT",
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetNextDescriptorHeapIndex(1),
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]
	);

	this->cbPerFrameData = new CB_PER_FRAME_STRUCT();

	this->InitRenderTasks();

	// temp, used to transmit textures/meshes to the default memory on GPU when loaded
	this->tempCommandInterface = new CommandInterface(this->device5, COMMAND_INTERFACE_TYPE::DIRECT_TYPE);
}

std::vector<Mesh*>* Renderer::LoadModel(std::wstring path)
{
	bool loadedBefore = false;
	std::vector<Mesh*>* meshes = AssetLoader::Get()->LoadModel(path, &loadedBefore);

	// ------------------------------ TEMPORARY CODE ------------------------------ 
	// Only Upload to default heaps if its the first time its loaded
	if (!loadedBefore)
	{
		for (Mesh* mesh : *meshes)
		{
			// Upload to Default heap
			mesh->UploadToDefault(
				this->device5,
				this->tempCommandInterface,
				this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
			this->WaitForGpu();

			// Wont upload data if its already up.. TEMPORARY safecheck inside the texture class
			for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
			{
				TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
				Texture* texture = mesh->GetTexture(type);
				texture->UploadToDefault(
					this->device5,
					this->tempCommandInterface,
					this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
				this->WaitForGpu();
			}
		}
	}
	return meshes;
}

Texture* Renderer::LoadTexture(std::wstring path)
{
	Texture* texture = AssetLoader::Get()->LoadTexture(path);

	if (texture == nullptr)
	{
		return nullptr;
	}

	// ------------------------------ TEMPORARY CODE ------------------------------ 
	// Wont upload data if its already up.. TEMPORARY safecheck inside the texture class
	texture->UploadToDefault(
		this->device5,
		this->tempCommandInterface,
		this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
	this->WaitForGpu();

	return texture;
}

// Handle the components thats used for rendering
void Renderer::SetSceneToDraw(Scene* scene)
{
	// Reset
	this->renderComponents.clear();
	for (auto& light : this->lights)
	{
		light.second.clear();
	}
	this->lightViewsPool->Clear();
	this->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	this->ScenePrimaryCamera = nullptr;
	this->wireFrameTask->Clear();
	this->boundingBoxesToBePicked.clear();

	// Handle and structure the components in the scene
#pragma region HandleComponents
	std::map<std::string, Entity*> entities = *scene->GetEntities();
	for (auto const& [entityName, entity] : entities)
	{
		// Only add the entities that actually should be drawn
		component::MeshComponent* mc = entity->GetComponent<component::MeshComponent>();
		if (mc != nullptr)
		{
			component::TransformComponent* tc = entity->GetComponent<component::TransformComponent>();
			if (tc != nullptr)
			{
				this->renderComponents.push_back(std::make_pair(mc, tc));

				// Send the Textures to GPU here later, so that textures aren't in memory if they aren't used
				// or submit index to a queue and then submit all textures together later..
			}
		}

		component::DirectionalLightComponent* dlc = entity->GetComponent<component::DirectionalLightComponent>();
		if (dlc != nullptr)
		{
			// Assign CBV from the lightPool
			ConstantBufferView* cbd = this->lightViewsPool->GetFreeConstantBufferView(LIGHT_TYPE::DIRECTIONAL_LIGHT);

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
			if(resolution != SHADOW_RESOLUTION::UNDEFINED)
			{

				si = this->lightViewsPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
				static_cast<DirectionalLight*>(dlc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

				ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(this->renderTasks[RENDER_TASK_TYPE::SHADOW]);
				srt->AddShadowCastingLight(std::make_pair(dlc, si));
			}

			// Save in renderer
			this->lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].push_back(std::make_tuple(dlc, cbd, si));
		}

		// Currently no shadows are implemented for pointLights
		component::PointLightComponent* plc = entity->GetComponent<component::PointLightComponent>();
		if (plc != nullptr)
		{
			// Assign resource from resourcePool
			ConstantBufferView* cbd = this->lightViewsPool->GetFreeConstantBufferView(LIGHT_TYPE::POINT_LIGHT);

			// Assign views required for shadows from the lightPool
			ShadowInfo* si = nullptr;

			// Save in renderer
			this->lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(plc, cbd, si));
		}

		component::SpotLightComponent* slc = entity->GetComponent<component::SpotLightComponent>();
		if (slc != nullptr)
		{
			// Assign resource from resourcePool
			ConstantBufferView* cbd = this->lightViewsPool->GetFreeConstantBufferView(LIGHT_TYPE::SPOT_LIGHT);

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
				si = this->lightViewsPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
				static_cast<SpotLight*>(slc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

				ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(this->renderTasks[RENDER_TASK_TYPE::SHADOW]);
				srt->AddShadowCastingLight(std::make_pair(slc, si));
			}
			// Save in renderer
			this->lights[LIGHT_TYPE::SPOT_LIGHT].push_back(std::make_tuple(slc, cbd, si));
		}

		component::CameraComponent* cc = entity->GetComponent<component::CameraComponent>();
		if (cc != nullptr)
		{
			if (cc->IsPrimary() == true)
			{
				this->ScenePrimaryCamera = cc->GetCamera();
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
					continue;
				}

				// Upload to Default heap
				m->UploadToDefault(
					this->device5,
					this->tempCommandInterface,
					this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
				this->WaitForGpu();

				bbc->SetMesh(m);

				this->wireFrameTask->AddObjectToDraw(&std::make_pair(m, bbc->GetTransform()));
			}

			// Add to vector so the mouse picker can check for intersections
			if (bbc->CanBePicked() == true)
			{
				this->boundingBoxesToBePicked.push_back(bbc);
				
			}
		}
	}
#pragma endregion HandleComponents
	
	// Setup Per-scene data and send to GPU
#pragma region Prepare_CbPerScene
	CB_PER_SCENE_STRUCT cps = {};
		// ----- directional lights -----
		cps.Num_Dir_Lights = this->lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].size();
		unsigned int index = 0;
		for (auto& tuple : this->lights[LIGHT_TYPE::DIRECTIONAL_LIGHT])
		{
			cps.dirLightIndices[index].x = std::get<1>(tuple)->GetDescriptorHeapIndex();
			index++;
		}
		// ----- directional lights -----

		// ----- point lights -----
		cps.Num_Point_Lights = this->lights[LIGHT_TYPE::POINT_LIGHT].size();
		index = 0;
		for (auto& tuple : this->lights[LIGHT_TYPE::POINT_LIGHT])
		{
			cps.pointLightIndices[index].x = std::get<1>(tuple)->GetDescriptorHeapIndex();
			index++;
		}
		// ----- point lights -----

		// ----- spot lights -----
		cps.Num_Spot_Lights = this->lights[LIGHT_TYPE::SPOT_LIGHT].size();
		index = 0;
		for (auto& tuple : this->lights[LIGHT_TYPE::SPOT_LIGHT])
		{
			cps.spotLightIndices[index].x = std::get<1>(tuple)->GetDescriptorHeapIndex();
			index++;
		}
		// ----- spot lights -----

	// Upload CB_PER_SCENE to defaultheap
	this->TempCopyResource( 
		this->cbPerScene->GetUploadResource(),
		this->cbPerScene->GetCBVResource(),
		&cps);
#pragma endregion Prepare_CbPerScene

	// Add Per-frame data to the copy queue
#pragma region Prepare_CbPerFrame

	// Lights
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		for (auto& tuple : this->lights[type])
		{
			void* data = std::get<0>(tuple)->GetLightData();
			ConstantBufferView* cbd = std::get<1>(tuple);
			this->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Submit(&std::make_pair(data, cbd));
		}
	}

	// CB_PER_FRAME_STRUCT
	void* perFrameData = static_cast<void*>(this->cbPerFrameData);
	this->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Submit(&std::make_pair(perFrameData, this->cbPerFrame));
#pragma endregion Prepare_CbPerFrame

	// -------------------- DEBUG STUFF --------------------
	// Test to change camera to the shadow casting lights cameras
	//auto& tuple = this->lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].at(0);
	//BaseCamera* tempCam = std::get<0>(tuple)->GetCamera();
	//this->ScenePrimaryCamera = tempCam;

	if (this->ScenePrimaryCamera == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "No primary camera was set in scene: %s\n", scene->GetName());
	}
	this->mousePicker->SetPrimaryCamera(this->ScenePrimaryCamera);
	scene->SetPrimaryCamera(this->ScenePrimaryCamera);
	this->SetRenderTasksRenderComponents();
	this->SetRenderTasksPrimaryCamera();

	this->currActiveScene = scene;
}

void Renderer::Update(double dt)
{
	// Update CB_PER_FRAME data
	this->cbPerFrameData->camPos = this->ScenePrimaryCamera->GetPositionFloat3();

	// Picking
	this->UpdateMousePicker();
	
	// Update scene
	this->currActiveScene->UpdateScene(dt);
}

void Renderer::SortObjectsByDistance()
{
	struct DistFromCamera
	{
		double distance;
		component::MeshComponent* mc;
		component::TransformComponent* tc;
	};

	int numRenderComponents = this->renderComponents.size();

	DistFromCamera* distFromCamArr = new DistFromCamera[numRenderComponents];

	// Get all the distances of each objects and store them by ID and distance
	XMFLOAT3 camPos = this->ScenePrimaryCamera->GetPosition();
	for (int i = 0; i < numRenderComponents; i++)
	{
		XMFLOAT3 objectPos = this->renderComponents.at(i).second->GetTransform()->GetPositionXMFLOAT3();

		double distance = sqrt(	pow(camPos.x - objectPos.x, 2) +
								pow(camPos.y - objectPos.y, 2) +
								pow(camPos.z - objectPos.z, 2));

		// Save the object alongside its distance to the camera
		distFromCamArr[i].distance = distance;
		distFromCamArr[i].mc = this->renderComponents.at(i).first;
		distFromCamArr[i].tc = this->renderComponents.at(i).second;
	}

	// InsertionSort (because its best case is O(N)), 
	// and since this is sorted ((((((EVERY FRAME)))))) this is a good choice of sorting algorithm
	int j = 0;
	DistFromCamera distFromCamArrTemp = {};
	for (int i = 1; i < numRenderComponents; i++)
	{
		j = i;
		while (j > 0 && (distFromCamArr[j - 1].distance > distFromCamArr[j].distance))
		{
			// Swap
			distFromCamArrTemp = distFromCamArr[j - 1];
			distFromCamArr[j - 1] = distFromCamArr[j];
			distFromCamArr[j] = distFromCamArrTemp;
			j--;
		}
	}

	// Fill the vector with sorted array
	this->renderComponents.clear();
	for (int i = 0; i < numRenderComponents; i++)
	{
		this->renderComponents.push_back(std::make_pair(distFromCamArr[i].mc, distFromCamArr[i].tc));
	}

	// Free memory
	delete distFromCamArr;

	// Update the entity-arrays inside the rendertasks
	this->SetRenderTasksRenderComponents();
}

void Renderer::Execute()
{
	IDXGISwapChain4* dx12SwapChain = this->swapChain->GetDX12SwapChain();
	int backBufferIndex = dx12SwapChain->GetCurrentBackBufferIndex();
	int commandInterfaceIndex = this->frameCounter++ % 2;

	/* --------------------- Record copy command lists --------------------- */
	// Copy per frame
	CopyTask* ct = this->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME];
	ct->SetCommandInterfaceIndex(commandInterfaceIndex);
	//this->threadpool->AddTask(ct, THREAD_FLAG::COPY_DATA);
	ct->Execute();
	//this->threadpool->WaitForThreads(THREAD_FLAG::COPY_DATA);


	/* --------------------- Execute copy command lists --------------------- */
	// Copy per frame
	this->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(
		this->copyCommandLists[commandInterfaceIndex].size(),
		this->copyCommandLists[commandInterfaceIndex].data());
	UINT64 copyFenceValue = ++this->fenceFrameValue;
	this->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->Signal(this->fenceFrame, copyFenceValue);

	/* --------------------- Record direct commandlists --------------------- */
	for (RenderTask* renderTask : this->renderTasks)
	{
		renderTask->SetBackBufferIndex(backBufferIndex);
		renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		this->threadPool->AddTask(renderTask, FLAG_THREAD::RENDER);
		//renderTask->Execute();	// NON-MULTITHREADED-VERSION 
	}
	
	if (DRAWBOUNDINGBOX == true)
	{
		this->wireFrameTask->SetBackBufferIndex(backBufferIndex);
		this->wireFrameTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		this->threadPool->AddTask(this->wireFrameTask, FLAG_THREAD::RENDER);
	}
	
	// Wait for the threads which records the commandlists to complete
	this->threadPool->WaitForThreads(FLAG_THREAD::RENDER | FLAG_THREAD::ALL);
	this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Wait(this->fenceFrame, copyFenceValue);

	this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->ExecuteCommandLists(
		this->directCommandLists[commandInterfaceIndex].size(), 
		this->directCommandLists[commandInterfaceIndex].data());

	/* --------------------------------------------------------------- */

	// Wait if the CPU is to far ahead of the gpu
	this->fenceFrameValue++;

	this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(this->fenceFrame, this->fenceFrameValue);
	this->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->Wait(this->fenceFrame, this->fenceFrameValue);
	WaitForFrame();

	HRESULT hr = dx12SwapChain->Present(0, 0);
	
#ifdef _DEBUG
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Swapchain Failed to present\n");
	}
#endif
}

void Renderer::SetRenderTasksPrimaryCamera()
{
	for (RenderTask* renderTask : this->renderTasks)
	{
		renderTask->SetCamera(this->ScenePrimaryCamera);
	}

	if (DRAWBOUNDINGBOX == true)
	{
		this->wireFrameTask->SetCamera(this->ScenePrimaryCamera);
	}
}

bool Renderer::CreateDevice()
{
	bool deviceCreated = false;

#ifdef _DEBUG
	//Enable the D3D12 debug layer.
	ID3D12Debug* debugController = nullptr;

#ifdef STATIC_LINK_DEBUGSTUFF
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	SafeRelease(debugController);
#else
	HMODULE mD3D12 = LoadLibrary(L"D3D12.dll"); // ist�llet f�r GetModuleHandle

	PFN_D3D12_GET_DEBUG_INTERFACE f = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(mD3D12, "D3D12GetDebugInterface");
	if (SUCCEEDED(f(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	SAFE_RELEASE(&debugController);
#endif
#endif

	IDXGIFactory6* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;

	CreateDXGIFactory(IID_PPV_ARGS(&factory));
	
	for (unsigned int adapterIndex = 0;; ++adapterIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter))
		{
			break; // No more adapters
		}
	
		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device5), nullptr)))
		{
			break;
		}
	
		SAFE_RELEASE(&adapter);
	}
	
	if (adapter)
	{
		HRESULT hr = S_OK;
		//Create the actual device.
		if (SUCCEEDED(hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device5))))
		{
			deviceCreated = true;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create Device\n");
		}
	
		SAFE_RELEASE(&adapter);
	}
	else
	{
		//Create warp device if no adapter was found.
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device5));
	}
	
	SAFE_RELEASE(&factory);

	return deviceCreated;
}

void Renderer::CreateCommandQueues()
{
	// Direct
	D3D12_COMMAND_QUEUE_DESC cqdDirect = {};
	cqdDirect.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hr;
	hr = device5->CreateCommandQueue(&cqdDirect, IID_PPV_ARGS(&this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]));
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Direct CommandQueue\n");
	}
	this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->SetName(L"DirectQueue");

	// Compute
	D3D12_COMMAND_QUEUE_DESC cqdCompute = {};
	cqdCompute.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	hr = device5->CreateCommandQueue(&cqdCompute, IID_PPV_ARGS(&this->commandQueues[COMMAND_INTERFACE_TYPE::COMPUTE_TYPE]));
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Compute CommandQueue\n");
	}
	this->commandQueues[COMMAND_INTERFACE_TYPE::COMPUTE_TYPE]->SetName(L"ComputeQueue");

	// Copy
	D3D12_COMMAND_QUEUE_DESC cqdCopy = {};
	cqdCopy.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	hr = device5->CreateCommandQueue(&cqdCopy, IID_PPV_ARGS(&this->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]));
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Copy CommandQueue\n");
	}
	this->commandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->SetName(L"CopyQueue");
}

void Renderer::CreateSwapChain(const HWND *hwnd)
{
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	this->swapChain = new SwapChain(
		device5,
		hwnd,
		width, height,
		this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE],
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV]);
}

void Renderer::CreateMainDSV(const HWND* hwnd)
{
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	this->mainDSV = new DepthStencilView(
		this->device5,
		width, height,	// width, height
		L"MainDSV_DEFAULT_RESOURCE",
		this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV],
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
}

void Renderer::CreateRootSignature()
{
	this->rootSignature = new RootSignature(this->device5);
}

void Renderer::UpdateMousePicker()
{
	this->mousePicker->UpdateRay();

	component::BoundingBoxComponent* pickedBoundingBox = nullptr;

	float tempDist;
	float closestDist = MAXNUMBER;

	for (component::BoundingBoxComponent* bbc : this->boundingBoxesToBePicked)
	{
		// Reset picked entities from last frame
		bbc->IsPickedThisFrame() = false;

		if (this->mousePicker->Pick(bbc, tempDist) == true)
		{
			if (tempDist < closestDist)
			{
				pickedBoundingBox = bbc;

				closestDist = tempDist;
			}
		}
	}

	if (closestDist < MAXNUMBER)
	{
		pickedBoundingBox->IsPickedThisFrame() = true;

		if (pickedBoundingBox->IsPickedThisFrame() == true)
		{
			//Log::Print("%s is picked! %d\n", pickedBoundingBox->GetParent()->GetName().c_str(), this->frameCounter);
		}
	}
}

void Renderer::InitRenderTasks()
{
#pragma region ForwardRendering
	/* Forward rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdForwardRender = {};
	gpsdForwardRender.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdForwardRender.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsdForwardRender.NumRenderTargets = 1;
	// Depthstencil usage
	gpsdForwardRender.SampleDesc.Count = 1;
	gpsdForwardRender.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdForwardRender.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdForwardRender.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdForwardRender.RasterizerState.FrontCounterClockwise = false;

	// Specify Blend descriptions
	D3D12_RENDER_TARGET_BLEND_DESC defaultRTdesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdForwardRender.BlendState.RenderTarget[i] = defaultRTdesc;

	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	gpsdForwardRender.DepthStencilState = dsd;
	gpsdForwardRender.DSVFormat = this->mainDSV->GetDXGIFormat();

	/* Forward rendering with stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdForwardRenderStencilTest = gpsdForwardRender;

	// Only change stencil testing
	dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// DepthStencil
	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xff;
	dsd.StencilWriteMask = 0xff;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOP = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_REPLACE, D3D12_COMPARISON_FUNC_ALWAYS };
	dsd.FrontFace = defaultStencilOP;
	dsd.BackFace = defaultStencilOP;

	gpsdForwardRenderStencilTest.DepthStencilState = dsd;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdForwardRenderVector;
	gpsdForwardRenderVector.push_back(&gpsdForwardRender);
	gpsdForwardRenderVector.push_back(&gpsdForwardRenderStencilTest);

	RenderTask* forwardRenderTask = new FowardRenderTask(
		this->device5,
		this->rootSignature, 
		L"ForwardVertex.hlsl", L"ForwardPixel.hlsl", 
		&gpsdForwardRenderVector, 
		L"ForwardRenderingPSO");

	forwardRenderTask->AddResource("cbPerFrame", this->cbPerFrame->GetCBVResource());
	forwardRenderTask->AddResource("cbPerScene", this->cbPerScene->GetCBVResource());
	forwardRenderTask->AddRenderTarget("swapChain", this->swapChain);
	forwardRenderTask->SetDescriptorHeaps(this->descriptorHeaps);
	

#pragma endregion ForwardRendering
#pragma region ModelOutlining
	
#pragma endregion ModelOutlining
#pragma region Blend
	// ------------------------ TASK 2: BLEND ---------------------------- FRONTCULL

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdBlendFrontCull = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdBlendBackCull = {};
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdBlendVector;

	gpsdBlendFrontCull.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdBlendFrontCull.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsdBlendFrontCull.NumRenderTargets = 1;
	// Depthstencil usage
	gpsdBlendFrontCull.SampleDesc.Count = 1;
	gpsdBlendFrontCull.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdBlendFrontCull.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdBlendFrontCull.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;

	// Specify Blend descriptions
	D3D12_RENDER_TARGET_BLEND_DESC blendRTdesc{};
	blendRTdesc.BlendEnable = true;
	blendRTdesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendRTdesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendRTdesc.BlendOp = D3D12_BLEND_OP_ADD;
	blendRTdesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendRTdesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blendRTdesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendRTdesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdBlendFrontCull.BlendState.RenderTarget[i] = blendRTdesc;


	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC dsdBlend = {};
	dsdBlend.DepthEnable = true;
	dsdBlend.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	dsdBlend.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	// Om pixels depth är lägre än den gamla så ritas den nya ut

	// DepthStencil
	dsdBlend.StencilEnable = false;
	dsdBlend.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	dsdBlend.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC blendStencilOP{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	dsdBlend.FrontFace = blendStencilOP;
	dsdBlend.BackFace = blendStencilOP;

	gpsdBlendFrontCull.DepthStencilState = dsdBlend;
	gpsdBlendFrontCull.DSVFormat = this->mainDSV->GetDXGIFormat();

	// ------------------------ TASK 2: BLEND ---------------------------- BACKCULL

	gpsdBlendBackCull.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdBlendBackCull.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsdBlendBackCull.NumRenderTargets = 1;
	// Depthstencil usage
	gpsdBlendBackCull.SampleDesc.Count = 1;
	gpsdBlendBackCull.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdBlendBackCull.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdBlendBackCull.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdBlendBackCull.BlendState.RenderTarget[i] = blendRTdesc;

	// DepthStencil
	dsdBlend.StencilEnable = false;
	dsdBlend.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpsdBlendBackCull.DepthStencilState = dsdBlend;
	gpsdBlendBackCull.DSVFormat = this->mainDSV->GetDXGIFormat();

	gpsdBlendVector.push_back(&gpsdBlendFrontCull);
	gpsdBlendVector.push_back(&gpsdBlendBackCull);

	RenderTask* blendRenderTask = new BlendRenderTask(this->device5,
		this->rootSignature,
		L"BlendVertex.hlsl",
		L"BlendPixel.hlsl",
		&gpsdBlendVector,
		L"BlendPSO");

	blendRenderTask->AddResource("cbPerFrame", this->cbPerFrame->GetCBVResource());
	blendRenderTask->AddResource("cbPerScene", this->cbPerScene->GetCBVResource());
	blendRenderTask->AddRenderTarget("swapChain", this->swapChain);
	blendRenderTask->SetDescriptorHeaps(this->descriptorHeaps);
	

#pragma endregion Blend
#pragma region ShadowPass
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdShadow = { 0 };
	gpsdShadow.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdShadow.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	gpsdShadow.NumRenderTargets = 0;
	// Depthstencil usage
	gpsdShadow.SampleDesc.Count = 1;
	gpsdShadow.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdShadow.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdShadow.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdShadow.RasterizerState.DepthBias = 0;
	gpsdShadow.RasterizerState.DepthBiasClamp = 0.0f;
	gpsdShadow.RasterizerState.SlopeScaledDepthBias = 0.0f;
	gpsdShadow.RasterizerState.FrontCounterClockwise = false;

	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC dsdShadow = {};
	dsdShadow.DepthEnable = true;
	dsdShadow.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsdShadow.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsdShadow.StencilEnable = false;

	gpsdShadow.DepthStencilState = dsdShadow;
	gpsdShadow.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Specify Blend descriptions
	D3D12_RENDER_TARGET_BLEND_DESC defaultShadowDesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdShadow.BlendState.RenderTarget[i] = defaultShadowDesc;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdShadowVector;
	gpsdShadowVector.push_back(&gpsdShadow);

	RenderTask* shadowRenderTask = new ShadowRenderTask(
		this->device5,
		this->rootSignature,
		L"ShadowVertex.hlsl", L"ShadowPixel.hlsl",
		&gpsdShadowVector,
		L"ShadowPSO");

	shadowRenderTask->SetDescriptorHeaps(this->descriptorHeaps);
#pragma endregion ShadowPass
#pragma region WireFrame
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdWireFrame = {};
	gpsdWireFrame.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdWireFrame.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsdWireFrame.NumRenderTargets = 1;
	// Depthstencil usage
	gpsdWireFrame.SampleDesc.Count = 1;
	gpsdWireFrame.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdWireFrame.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpsdWireFrame.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpsdWireFrame.RasterizerState.FrontCounterClockwise = false;

	// Specify Blend descriptions
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdWireFrame.BlendState.RenderTarget[i] = defaultRTdesc;


	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdWireFrameVector;
	gpsdWireFrameVector.push_back(&gpsdWireFrame);

	this->wireFrameTask = new WireframeRenderTask(this->device5,
		this->rootSignature,
		L"WhiteVertex.hlsl", L"WhitePixel.hlsl",
		&gpsdWireFrameVector,
		L"WireFramePSO");

	this->wireFrameTask->AddRenderTarget("swapChain", this->swapChain);
	this->wireFrameTask->SetDescriptorHeaps(this->descriptorHeaps);
#pragma endregion WireFrame

	CopyTask* copyPerFrameTask = new CopyPerFrameTask(this->device5);

	// Add the tasks to desired vectors so they can be used in renderer
	/* -------------------------------------------------------------- */


	/* ------------------------- CopyQueue Tasks ------------------------ */

	this->copyTasks[COPY_TASK_TYPE::COPY_PER_FRAME] = copyPerFrameTask;

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		this->copyCommandLists[i].push_back(copyPerFrameTask->GetCommandList(i));

	/* ------------------------- ComputeQueue Tasks ------------------------ */

	// None atm

	/* ------------------------- DirectQueue Tasks ---------------------- */
	this->renderTasks[RENDER_TASK_TYPE::SHADOW] = shadowRenderTask;
	this->renderTasks[RENDER_TASK_TYPE::FORWARD_RENDER] = forwardRenderTask;
	this->renderTasks[RENDER_TASK_TYPE::BLEND] = blendRenderTask;

	// Pushback in the order of execution
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		this->directCommandLists[i].push_back(shadowRenderTask->GetCommandList(i));

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		this->directCommandLists[i].push_back(forwardRenderTask->GetCommandList(i));

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		this->directCommandLists[i].push_back(blendRenderTask->GetCommandList(i));

	if (DRAWBOUNDINGBOX == true)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
			this->directCommandLists[i].push_back(this->wireFrameTask->GetCommandList(i));
	}
}

void Renderer::SetRenderTasksRenderComponents()
{
	for (RenderTask* rendertask : this->renderTasks)
	{
		rendertask->SetRenderComponents(&this->renderComponents);
	}
}

void Renderer::CreateDescriptorHeaps()
{
	this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV] = new DescriptorHeap(this->device5, DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV);
	this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV] = new DescriptorHeap(this->device5, DESCRIPTOR_HEAP_TYPE::RTV);
	this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV] = new DescriptorHeap(this->device5, DESCRIPTOR_HEAP_TYPE::DSV);
}

void Renderer::CreateFences()
{
	HRESULT hr = device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fenceFrame));

	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Fence\n");
	}
	this->fenceFrameValue = 1;

	// Event handle to use for GPU synchronization
	this->eventHandle = CreateEvent(0, false, false, 0);
}

void Renderer::WaitForFrame(unsigned int framesToBeAhead)
{
	static constexpr unsigned int nrOfFenceChangesPerFrame = 2;
	unsigned int fenceValuesToBeAhead = framesToBeAhead * nrOfFenceChangesPerFrame;

	//Wait if the CPU is "framesToBeAhead" number of frames ahead of the GPU
	if (this->fenceFrame->GetCompletedValue() < this->fenceFrameValue - fenceValuesToBeAhead)
	{
		this->fenceFrame->SetEventOnCompletion(this->fenceFrameValue - fenceValuesToBeAhead, this->eventHandle);
		WaitForSingleObject(this->eventHandle, INFINITE);
	}
}

void Renderer::WaitForGpu()
{
	//Signal and increment the fence value.
	const UINT64 oldFenceValue = this->fenceFrameValue;
	this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(this->fenceFrame, oldFenceValue);
	this->fenceFrameValue++;

	//Wait until command queue is done.
	if (this->fenceFrame->GetCompletedValue() < oldFenceValue)
	{
		this->fenceFrame->SetEventOnCompletion(oldFenceValue, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
	}
}

void Renderer::TempCopyResource(Resource* uploadResource, Resource* defaultResource, void* data)
{
	this->tempCommandInterface->Reset(0);
	// Set the data into the upload heap
	uploadResource->SetData(data);

	this->tempCommandInterface->GetCommandList(0)->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultResource->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST));

	// To Defaultheap from Uploadheap
	this->tempCommandInterface->GetCommandList(0)->CopyResource(
		defaultResource->GetID3D12Resource1(),	// Receiever
		uploadResource->GetID3D12Resource1());	// Sender

	this->tempCommandInterface->GetCommandList(0)->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultResource->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON));

	this->tempCommandInterface->GetCommandList(0)->Close();
	ID3D12CommandList* ppCommandLists[] = { this->tempCommandInterface->GetCommandList(0) };
	// this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Wait(this->fenceFrame, this->fenceFrameValue - 1);
	this->commandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->ExecuteCommandLists(ARRAYSIZE(ppCommandLists), ppCommandLists);
	this->WaitForGpu();
}
