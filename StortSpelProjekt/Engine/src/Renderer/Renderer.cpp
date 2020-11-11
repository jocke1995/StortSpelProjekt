#include "stdafx.h"
#include "Renderer.h"

// Misc
#include "../Misc/MultiThreading/ThreadPool.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/MultiThreading/Thread.h"
#include "../Misc/Window.h"
#include "../Misc/Option.h"
#include "../Misc/GUI2DElements/Text.h"
#include "../Misc/GUI2DElements/Font.h"

// ECS
#include "../ECS/Scene.h"
#include "../ECS/Entity.h"
#include "../ECS/Components/GUI2DComponent.h"
#include "../ECS/Components/ModelComponent.h"
#include "../ECS/Components/SkyboxComponent.h"
#include "../ECS/Components/ParticleEmitterComponent.h"
#include "../ECS/Components/BoundingBoxComponent.h"
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/Lights/DirectionalLightComponent.h"
#include "../ECS/Components/Lights/PointLightComponent.h"
#include "../ECS/Components/Lights/SpotLightComponent.h"


// Renderer-Engine 
#include "RootSignature.h"
#include "SwapChain.h"
#include "GPUMemory/DepthStencilView.h"
#include "ViewPool.h"
#include "BoundingBoxPool.h"
#include "CommandInterface.h"
#include "DescriptorHeap.h"
#include "Transform.h"
#include "Camera/BaseCamera.h"
#include "Model.h"
#include "AnimatedModel.h"
#include "Mesh.h"
#include "AnimatedMesh.h"
#include "Texture/Texture.h"
#include "Texture/Texture2D.h"
#include "Texture/Texture2DGUI.h"
#include "Texture/TextureCubeMap.h"
#include "Material.h"

// GPUMemory
#include "GPUMemory/Resource.h"
#include "GPUMemory/ConstantBuffer.h"
#include "GPUMemory/UnorderedAccess.h"
// Views
#include "GPUMemory/ShaderResourceView.h"
#include "GPUMemory/ConstantBufferView.h"
#include "GPUMemory/DepthStencil.h"

// Techniques
#include "Bloom.h"
#include "PingPongResource.h"
#include "ShadowInfo.h"
#include "MousePicker.h"

// Graphics
#include "DX12Tasks/DepthRenderTask.h"
#include "DX12Tasks/AnimatedDepthRenderTask.h"
#include "DX12Tasks/WireframeRenderTask.h"
#include "DX12Tasks/OutliningRenderTask.h"
#include "DX12Tasks/ForwardRenderTask.h"
#include "DX12Tasks/TransparentRenderTask.h"
#include "DX12Tasks/ShadowRenderTask.h"
#include "DX12Tasks/DownSampleRenderTask.h"
#include "DX12Tasks/MergeRenderTask.h"
#include "DX12Tasks/TextTask.h"
#include "DX12Tasks/ImGuiRenderTask.h"
#include "DX12Tasks/SkyboxRenderTask.h"
#include "DX12Tasks/QuadTask.h"
#include "DX12Tasks/ParticleRenderTask.h"

// Copy 
#include "DX12Tasks/CopyPerFrameTask.h"
#include "DX12Tasks/CopyOnDemandTask.h"

// Compute
#include "DX12Tasks/BlurComputeTask.h"

// Event
#include "../Events/EventBus.h"

//ImGui
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "../ImGUI/imgui_impl_dx12.h"
#include "../ImGUI/ImGuiHandler.h"

Renderer::Renderer()
{
	EventBus::GetInstance().Subscribe(this, &Renderer::toggleFullscreen);
	m_RenderTasks.resize(RENDER_TASK_TYPE::NR_OF_RENDERTASKS);
	m_CopyTasks.resize(COPY_TASK_TYPE::NR_OF_COPYTASKS);
	m_ComputeTasks.resize(COMPUTE_TASK_TYPE::NR_OF_COMPUTETASKS);
}

Renderer& Renderer::GetInstance()
{
	static Renderer instance;
	return instance;
}

Renderer::~Renderer()
{
	// Cleanup ImGui
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::DeleteDxResources()
{
	Log::Print("----------------------------  Deleting Renderer  ----------------------------------\n");
	waitForGPU();

	SAFE_RELEASE(&m_pFenceFrame);
	if (!CloseHandle(m_EventHandle))
	{
		Log::Print("Failed To Close Handle... ErrorCode: %d\n", GetLastError());
	}

	SAFE_RELEASE(&m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]);
	SAFE_RELEASE(&m_CommandQueues[COMMAND_INTERFACE_TYPE::COMPUTE_TYPE]);
	SAFE_RELEASE(&m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]);

	delete m_pRootSignature;
	delete m_pFullScreenQuad;
	delete m_pSwapChain;
	delete m_pBloomResources;
	delete m_pMainDepthStencil;

	for (auto& pair : m_DescriptorHeaps)
	{
		delete pair.second;
	}

	for (ComputeTask* computeTask : m_ComputeTasks)
		delete computeTask;

	for (CopyTask* copyTask : m_CopyTasks)
		delete copyTask;

	for (RenderTask* renderTask : m_RenderTasks)
		delete renderTask;

	SAFE_RELEASE(&m_pDevice5);

	delete m_pMousePicker;

	delete m_pViewPool;
	delete m_pCbPerScene;
	delete m_pCbPerSceneData;
	delete m_pCbPerFrame;
	delete m_pCbPerFrameData;
}

void Renderer::InitD3D12(const Window *window, HINSTANCE hInstance, ThreadPool* threadPool)
{
	m_pThreadPool = threadPool;
	m_pWindow = window;

	// Create Device
	if (!createDevice())
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Device\n");
	}

	// Create CommandQueues (copy, compute and direct)
	createCommandQueues();

	// Create DescriptorHeaps
	createDescriptorHeaps();

	// Fence for WaitForFrame();
	createFences();

	// Rendertargets
	createSwapChain();
	m_pBloomResources = new Bloom(m_pDevice5, 
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
		m_pSwapChain
		);

	// Create Main DepthBuffer
	createMainDSV();

	// Picking
	m_pMousePicker = new MousePicker();
	
	// Create Rootsignature
	createRootSignature();

	// FullScreenQuad
	createFullScreenQuad();

	// Init Assetloader
	AssetLoader* al = AssetLoader::Get(m_pDevice5, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV], m_pWindow);

	// Init BoundingBoxPool
	BoundingBoxPool::Get(m_pDevice5, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
	
	// Pool to handle GPU memory for the lights
	m_pViewPool = new ViewPool(
		m_pDevice5,
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV]);

	// Allocate memory for cbPerScene
	m_pCbPerScene = new ConstantBuffer(
		m_pDevice5, 
		sizeof(CB_PER_SCENE_STRUCT),
		L"CB_PER_SCENE",
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]
		);
	
	m_pCbPerSceneData = new CB_PER_SCENE_STRUCT();

	// Allocate memory for cbPerFrame
	m_pCbPerFrame = new ConstantBuffer(
		m_pDevice5,
		sizeof(CB_PER_FRAME_STRUCT),
		L"CB_PER_FRAME",
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]
	);

	m_pCbPerFrameData = new CB_PER_FRAME_STRUCT();

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	unsigned int imGuiTextureIndex = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetNextDescriptorHeapIndex(1);

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(*m_pWindow->GetHwnd());
	ImGui_ImplDX12_Init(m_pDevice5, NUM_SWAP_BUFFERS,
		DXGI_FORMAT_R16G16B16A16_FLOAT, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetID3D12DescriptorHeap(),
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetCPUHeapAt(imGuiTextureIndex),
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetGPUHeapAt(imGuiTextureIndex));

	initRenderTasks();

	submitMeshToCodt(m_pFullScreenQuad);
}

void Renderer::Update(double dt)
{
	
}

void Renderer::RenderUpdate(double dt)
{
	/* ------ ImGui ------*/
	if (DEVELOPERMODE_DEVINTERFACE == true)
	{
		ImGuiHandler::GetInstance().NewFrame();
	}
	
	float3 right = m_pScenePrimaryCamera->GetRightVectorFloat3();
	right.normalize();

	float3 forward = m_pScenePrimaryCamera->GetDirectionFloat3();
	forward.normalize();

	// TODO: fix camera up vector
	float3 up = forward.cross(right);
	up.normalize();

	// Update CB_PER_FRAME data
	m_pCbPerFrameData->camPos = m_pScenePrimaryCamera->GetPositionFloat3();
	m_pCbPerFrameData->camRight = right;
	m_pCbPerFrameData->camUp = up;
	m_pCbPerFrameData->camForward = forward;

	// Picking
	updateMousePicker();

	/* ------ ImGui ------*/
	if (DEVELOPERMODE_DEVINTERFACE == true)
	{
		ImGuiHandler::GetInstance().UpdateFrame();
	}
}

void Renderer::SortObjects()
{
	struct DistFromCamera
	{
		double distance;
		component::ModelComponent* mc;
		component::TransformComponent* tc;
	};

	for (auto& renderComponents : m_RenderComponents)
	{
		int numRenderComponents = renderComponents.second.size();

		DistFromCamera* distFromCamArr = new DistFromCamera[numRenderComponents];

		// Get all the distances of each objects and store them by ID and distance
		DirectX::XMFLOAT3 camPos = m_pScenePrimaryCamera->GetPosition();
		for (int i = 0; i < numRenderComponents; i++)
		{
			DirectX::XMFLOAT3 objectPos = renderComponents.second.at(i).second->GetTransform()->GetPositionXMFLOAT3();

			double distance = sqrt(pow(camPos.x - objectPos.x, 2) +
				pow(camPos.y - objectPos.y, 2) +
				pow(camPos.z - objectPos.z, 2));

			// Save the object alongside its distance to the m_pCamera
			distFromCamArr[i].distance = distance;
			distFromCamArr[i].mc = renderComponents.second.at(i).first;
			distFromCamArr[i].tc = renderComponents.second.at(i).second;
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
		renderComponents.second.clear();
		for (int i = 0; i < numRenderComponents; i++)
		{
			renderComponents.second.push_back(std::make_pair(distFromCamArr[i].mc, distFromCamArr[i].tc));
		}

		// Free memory
		delete distFromCamArr;
	}

	// Sort the quads by looking at their depths
	std::sort(m_QuadComponents.begin(), m_QuadComponents.end(), [](component::GUI2DComponent* a, component::GUI2DComponent* b)
	{
		return a->GetQuadManager()->GetDepth() < b->GetQuadManager()->GetDepth();
	});
	
	// Update the entity-arrays inside the rendertasks
	setRenderTasksGUI2DComponents();
	setRenderTasksRenderComponents();
}

void Renderer::Execute()
{
	IDXGISwapChain4* dx12SwapChain = m_pSwapChain->GetDX12SwapChain();
	int backBufferIndex = dx12SwapChain->GetCurrentBackBufferIndex();
	int commandInterfaceIndex = m_FrameCounter++ % 2;

	CopyTask* copyTask = nullptr;
	ComputeTask* computeTask = nullptr;
	RenderTask* renderTask = nullptr;
	/* --------------------- Record command lists --------------------- */

	// Copy on demand
	copyTask = m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND];
	copyTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(copyTask);

	// Copy per frame
	copyTask = m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME];
	copyTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(copyTask);

	// Depth pre-pass
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS];
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Animation Depth pre-pass
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::ANIMATION_DEPTH_PRE_PASS];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Recording shadowmaps
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::SHADOW];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Opaque draw
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// DownSample the texture used for bloom
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::DOWNSAMPLE];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Skybox
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::SKYBOX];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Blending with constant value
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_CONSTANT];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Blending with opacity texture
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_TEXTURE];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Blending with opacity texture
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::PARTICLE];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	// Blurring for bloom
	computeTask = m_ComputeTasks[COMPUTE_TASK_TYPE::BLUR];
	computeTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(computeTask);

	// Outlining, if an object is picked
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::OUTLINE];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	renderTask = m_RenderTasks[RENDER_TASK_TYPE::QUAD];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	renderTask = m_RenderTasks[RENDER_TASK_TYPE::TEXT];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);

	renderTask = m_RenderTasks[RENDER_TASK_TYPE::MERGE];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask);
	
	/* ----------------------------- DEVELOPERMODE CommandLists ----------------------------- */
	if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
	{
		renderTask = m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME];
		renderTask->SetBackBufferIndex(backBufferIndex);
		renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		m_pThreadPool->AddTask(renderTask);
	}

	if (DEVELOPERMODE_DEVINTERFACE == true)
	{
		renderTask = m_RenderTasks[RENDER_TASK_TYPE::IMGUI];
		renderTask->SetBackBufferIndex(backBufferIndex);
		renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		m_pThreadPool->AddTask(renderTask);
	}
	/* ----------------------------- DEVELOPERMODE CommandLists ----------------------------- */

	// Wait for the threads which records the commandlists to complete
	m_pThreadPool->WaitForThreads(FLAG_THREAD::RENDER);

	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->ExecuteCommandLists(
		m_DirectCommandLists[commandInterfaceIndex].size(), 
		m_DirectCommandLists[commandInterfaceIndex].data());

	/* --------------------------------------------------------------- */

	// Wait if the CPU is to far ahead of the gpu
	m_FenceFrameValue++;

	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(m_pFenceFrame, m_FenceFrameValue);
	waitForFrame();

	/*------------------- Post draw stuff -------------------*/
	// Clear copy on demand
	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Clear();

	/*------------------- Present -------------------*/
	HRESULT hr = dx12SwapChain->Present(0, 0);
	
#ifdef _DEBUG
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Swapchain Failed to present\n");
	}
#endif
}

void Renderer::InitSkyboxComponent(component::SkyboxComponent* component)
{
	Mesh* mesh = component->GetMesh();
	submitMeshToCodt(mesh);

	Texture* texture = static_cast<TextureCubeMap*>(component->GetTexture());
	submitTextureToCodt(texture);

	// Finally store the object in m_pRenderer so it will be drawn
	m_pSkyboxComponent = component;
}

void Renderer::InitModelComponent(component::ModelComponent* mc)
{
	component::TransformComponent* tc = mc->GetParent()->GetComponent<component::TransformComponent>();

	// Submit to codt
	submitModelToGPU(mc->m_pModel);
	
	// Only add the m_Entities that actually should be drawn
	if (tc != nullptr)
	{
		// Finally store the object in the corresponding renderComponent vectors so it will be drawn
		if (FLAG_DRAW::DRAW_TRANSPARENT_CONSTANT & mc->GetDrawFlag())
		{
			m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_CONSTANT].push_back(std::make_pair(mc, tc));
		}

		if (FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE & mc->GetDrawFlag())
		{
			m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE].push_back(std::make_pair(mc, tc));
		}

		if (FLAG_DRAW::DRAW_OPAQUE & mc->GetDrawFlag())
		{
			m_RenderComponents[FLAG_DRAW::DRAW_OPAQUE].push_back(std::make_pair(mc, tc));
		}
		else if (FLAG_DRAW::DRAW_ANIMATED & mc->GetDrawFlag())
		{
			// Depth pre-pass calculations of vertices
			m_RenderComponents[FLAG_DRAW::DRAW_ANIMATED].push_back(std::make_pair(mc, tc));

			// Opaque drawing as usual
			m_RenderComponents[FLAG_DRAW::DRAW_OPAQUE].push_back(std::make_pair(mc, tc));
		}

		if (FLAG_DRAW::NO_DEPTH & ~mc->GetDrawFlag())
		{
			m_RenderComponents[FLAG_DRAW::NO_DEPTH].push_back(std::make_pair(mc, tc));
		}

		if (FLAG_DRAW::GIVE_SHADOW & mc->GetDrawFlag())
		{
			m_RenderComponents[FLAG_DRAW::GIVE_SHADOW].push_back(std::make_pair(mc, tc));
		}
	}
}

void Renderer::InitDirectionalLightComponent(component::DirectionalLightComponent* component)
{
	// Assign CBV from the lightPool
	std::wstring resourceName = L"DirectionalLight";
	ConstantBuffer* cb = m_pViewPool->GetFreeCB(sizeof(DirectionalLight), resourceName);

	// Check if the light is to cast shadows
	SHADOW_RESOLUTION resolution = SHADOW_RESOLUTION::UNDEFINED;

	int shadowRes = -1;
	if (component->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW)
	{
		shadowRes = std::stoi(Option::GetInstance().GetVariable("i_shadowResolution").c_str());
	}

	if (shadowRes == 0)
	{
		resolution = SHADOW_RESOLUTION::LOW;
	}
	else if (shadowRes == 1)
	{
		resolution = SHADOW_RESOLUTION::MEDIUM;
	}
	else if (shadowRes >= 2)
	{
		resolution = SHADOW_RESOLUTION::HIGH;
	}

	// Assign views required for shadows from the lightPool
	ShadowInfo* si = nullptr;
	if (resolution != SHADOW_RESOLUTION::UNDEFINED)
	{
		si = m_pViewPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
		static_cast<DirectionalLight*>(component->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

		ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
		srt->AddShadowCastingLight(std::make_pair(component, si));
	}

	// Save in m_pRenderer
	m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].push_back(std::make_tuple(component, cb, si));

	// Submit to cbperframe
	
}

void Renderer::InitPointLightComponent(component::PointLightComponent* component)
{
	// Assign CBV from the lightPool
	std::wstring resourceName = L"PointLight";
	ConstantBuffer* cb = m_pViewPool->GetFreeCB(sizeof(PointLight), resourceName);

	// Assign views required for shadows from the lightPool
	ShadowInfo* si = nullptr;

	// Save in m_pRenderer
	m_Lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(component, cb, si));
}

void Renderer::InitSpotLightComponent(component::SpotLightComponent* component)
{
	// Assign CBV from the lightPool
	std::wstring resourceName = L"SpotLight";
	ConstantBuffer* cb = m_pViewPool->GetFreeCB(sizeof(SpotLight), resourceName);

	// Check if the light is to cast shadows
	SHADOW_RESOLUTION resolution = SHADOW_RESOLUTION::UNDEFINED;

	int shadowRes = -1;
	if (component->GetLightFlags() & FLAG_LIGHT::CAST_SHADOW)
	{
		shadowRes = std::stoi(Option::GetInstance().GetVariable("i_shadowResolution").c_str());
	}

	if (shadowRes == 0)
	{
		resolution = SHADOW_RESOLUTION::LOW;
	}
	else if (shadowRes == 1)
	{
		resolution = SHADOW_RESOLUTION::MEDIUM;
	}
	else if (shadowRes >= 2)
	{
		resolution = SHADOW_RESOLUTION::HIGH;
	}

	// Assign views required for shadows from the lightPool
	ShadowInfo* si = nullptr;
	if (resolution != SHADOW_RESOLUTION::UNDEFINED)
	{
		si = m_pViewPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
		static_cast<SpotLight*>(component->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

		ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
		srt->AddShadowCastingLight(std::make_pair(component, si));
	}
	// Save in m_pRenderer
	m_Lights[LIGHT_TYPE::SPOT_LIGHT].push_back(std::make_tuple(component, cb, si));
}

void Renderer::InitCameraComponent(component::CameraComponent* component)
{
	if (component->IsPrimary() == true)
	{
		m_pScenePrimaryCamera = component->GetCamera();
	}
}

void Renderer::InitBoundingBoxComponent(component::BoundingBoxComponent* component)
{
	// Add it to m_pTask so it can be drawn
	if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
	{
		for (unsigned int i = 0; i < component->GetNumBoundingBoxes(); i++)
		{
			Mesh* m = BoundingBoxPool::Get()->CreateBoundingBoxMesh(component->GetPathOfModel(i));
			if (m == nullptr)
			{
				Log::PrintSeverity(Log::Severity::WARNING, "Forgot to initialize BoundingBoxComponent on Entity: %s\n", component->GetParent()->GetName().c_str());
				return;
			}

			// TODO: don't load here, load in loadScene
			// Submit to GPU
			//LoadMesh(m);
			submitMeshToCodt(m);

			component->AddMesh(m);
		}
		static_cast<WireframeRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME])->AddObjectToDraw(component);
	}

	// Add to vector so the mouse picker can check for intersections
	if (component->GetFlagOBB() & F_OBBFlags::PICKING)
	{
		m_BoundingBoxesToBePicked.push_back(component);
	}
}

void Renderer::InitGUI2DComponent(component::GUI2DComponent* component)
{
	auto* textDataMap = component->GetTextManager()->GetTextDataMap();
	auto* quad = component->GetQuadManager()->GetQuad();

	if (textDataMap != nullptr)
	{
		for (auto textData : *textDataMap)
		{
			component->GetTextManager()->uploadTextData(textData.first, this);
		}

		// Finally store the text in m_pRenderer so it will be drawn
		m_TextComponents.push_back(component);
	}

	if (quad != nullptr)
	{
		component->GetQuadManager()->uploadQuadData(this);

		// Finally store the quad in m_pRenderer so it will be drawn
		m_QuadComponents.push_back(component);
	}
}

void Renderer::InitParticleEmitterComponent(component::ParticleEmitterComponent* component)
{
	auto mc = nullptr; // Particles don't have support for meshcomponent
	auto tc = component->GetParent()->GetComponent<component::TransformComponent>();
	
	Texture* texture = static_cast<Texture*>(component->GetTexture());
	submitTextureToCodt(texture);

	if (tc != nullptr)
	{
		m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE].push_back(std::make_pair(mc, tc));
	}
}

void Renderer::UnInitSkyboxComponent(component::SkyboxComponent* component)
{
}

void Renderer::UnInitModelComponent(component::ModelComponent* component)
{
	// Remove component from renderComponents
	// TODO: change data structure to allow O(1) add and remove
	for (auto& renderComponent : m_RenderComponents)
	{
		for (int i = 0; i < renderComponent.second.size(); i++)
		{
			// Remove from all renderComponent-vectors if they are there
			component::ModelComponent* comp = nullptr;
			comp = renderComponent.second[i].first;
			if (comp == component)
			{
				renderComponent.second.erase(renderComponent.second.begin() + i);
			}
		}
	}

	// Update Render Tasks components (forward the change in renderComponents)
	setRenderTasksRenderComponents();
}

void Renderer::UnInitDirectionalLightComponent(component::DirectionalLightComponent* component)
{
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		unsigned int j = 0;

		for (auto& tuple : m_Lights[type])
		{
			Light* light = std::get<0>(tuple);

			component::DirectionalLightComponent* dlc = static_cast<component::DirectionalLightComponent*>(light);

			// Remove light if it matches the entity
			if (component == dlc)
			{
				// Free memory so other m_Entities can use it
				ConstantBuffer* cbv = std::get<1>(tuple);
				ShadowInfo* si = std::get<2>(tuple);
				m_pViewPool->ClearSpecificLight(type, cbv, si);

				// Remove from CopyPerFrame
				CopyPerFrameTask* cpft = nullptr;
				cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
				cpft->ClearSpecific(cbv->GetUploadResource());

				// Finally remove from m_pRenderer
				ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
				srt->ClearSpecificLight(std::get<0>(tuple));
				m_Lights[type].erase(m_Lights[type].begin() + j);

				// Update cbPerScene
				SubmitUploadPerSceneData();
				break;
			}
			j++;
		}
	}
}

void Renderer::UnInitPointLightComponent(component::PointLightComponent* component)
{
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		unsigned int j = 0;

		for (auto& tuple : m_Lights[type])
		{
			Light* light = std::get<0>(tuple);

			component::PointLightComponent* plc = static_cast<component::PointLightComponent*>(light);

			// Remove light if it matches the entity
			if (component == plc)
			{
				// Free memory so other m_Entities can use it
				ConstantBuffer* cbv = std::get<1>(tuple);
				ShadowInfo* si = std::get<2>(tuple);
				m_pViewPool->ClearSpecificLight(type, cbv, si);

				// Remove from CopyPerFrame
				CopyPerFrameTask* cpft = nullptr;
				cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
				cpft->ClearSpecific(cbv->GetUploadResource());

				// Finally remove from m_pRenderer
				ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
				srt->ClearSpecificLight(std::get<0>(tuple));
				m_Lights[type].erase(m_Lights[type].begin() + j);

				// Update cbPerScene
				SubmitUploadPerSceneData();
				break;
			}
			j++;
		}
	}
}

void Renderer::UnInitSpotLightComponent(component::SpotLightComponent* component)
{
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		unsigned int j = 0;

		for (auto& tuple : m_Lights[type])
		{
			Light* light = std::get<0>(tuple);

			component::SpotLightComponent* slc = static_cast<component::SpotLightComponent*>(light);

			// Remove light if it matches the entity
			if (component == slc)
			{
				// Free memory so other m_Entities can use it
				ConstantBuffer* cbv = std::get<1>(tuple);
				ShadowInfo* si = std::get<2>(tuple);
				m_pViewPool->ClearSpecificLight(type, cbv, si);

				// Remove from CopyPerFrame
				CopyPerFrameTask* cpft = nullptr;
				cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
				cpft->ClearSpecific(cbv->GetUploadResource());

				// Finally remove from m_pRenderer
				ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
				srt->ClearSpecificLight(std::get<0>(tuple));
				m_Lights[type].erase(m_Lights[type].begin() + j);

				// Update cbPerScene
				SubmitUploadPerSceneData();
				break;
			}
			j++;
		}
	}
}

void Renderer::UnInitCameraComponent(component::CameraComponent* component)
{
}

void Renderer::UnInitBoundingBoxComponent(component::BoundingBoxComponent* component)
{
	// Check if the entity got a boundingbox component.
	if (component != nullptr)
	{
		if (component->GetParent() != nullptr)
		{
			// Stop drawing the wireFrame
			if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
			{
				static_cast<WireframeRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME])->ClearSpecific(component);
			}

			// Stop picking this boundingBox
			unsigned int i = 0;
			for (auto& bbcToBePicked : m_BoundingBoxesToBePicked)
			{
				if (bbcToBePicked == component)
				{
					m_BoundingBoxesToBePicked.erase(m_BoundingBoxesToBePicked.begin() + i);
					break;
				}
				i++;
			}
		}
	}
}

void Renderer::UnInitGUI2DComponent(component::GUI2DComponent* component)
{
	waitForGPU();

	// Remove component from textComponents
	// TODO: change data structure to allow O(1) add and remove
	for (int i = 0; i < m_TextComponents.size(); i++)
	{
		// Remove from all textComponent-vectors if they are there
		component::GUI2DComponent* comp = m_TextComponents.at(i);
		if (comp == component)
		{
			m_TextComponents.erase(m_TextComponents.begin() + i);
		}
	}

	// Remove component from quadComponents
	// TODO: change data structure to allow O(1) add and remove
	for (int i = 0; i < m_QuadComponents.size(); i++)
	{
		// Remove from all quadComponent-vectors if they are there
		component::GUI2DComponent* comp = m_QuadComponents.at(i);
		if (comp == component)
		{
			m_QuadComponents.erase(m_QuadComponents.begin() + i);
		}
	}

	setRenderTasksGUI2DComponents();
}

void Renderer::UnitParticleEmitterComponent(component::ParticleEmitterComponent* component)
{
	auto tc = component->GetParent()->GetComponent<component::TransformComponent>();

	// Remove component from renderComponents
	// TODO: change data structure to allow O(1) add and remove
	auto renderComponents = m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE];

	auto it = renderComponents.begin();
	while (it != renderComponents.end())
	{
		// Remove from all renderComponent-vectors if they are there
		component::TransformComponent* tcComp = nullptr;
		tcComp = (*it).second;
		if (tcComp == tc)
		{
			it = renderComponents.erase(it);
		}

		++it;
	}
}

void Renderer::OnResetScene()
{
	m_RenderComponents.clear();
	for (auto& light : m_Lights)
	{
		light.second.clear();
	}
	m_pViewPool->ClearAll();
	m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]->Clear();
	static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW])->Clear();
	m_pScenePrimaryCamera = nullptr;
	static_cast<WireframeRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME])->Clear();
	m_BoundingBoxesToBePicked.clear();
	m_QuadComponents.clear();
	m_TextComponents.clear();
}

void Renderer::submitToCodt(std::tuple<Resource*, Resource*, const void*>* Upload_Default_Data)
{
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
	codt->Submit(Upload_Default_Data);
}

void Renderer::submitMeshToCodt(Mesh* mesh)
{
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);

	std::tuple<Resource*, Resource*, const void*> Vert_Upload_Default_Data(mesh->m_pUploadResourceVertices, mesh->m_pDefaultResourceVertices, mesh->m_Vertices.data());
	std::tuple<Resource*, Resource*, const void*> Indi_Upload_Default_Data(mesh->m_pUploadResourceIndices, mesh->m_pDefaultResourceIndices, mesh->m_Indices.data());

	codt->Submit(&Vert_Upload_Default_Data);
	codt->Submit(&Indi_Upload_Default_Data);
}

void Renderer::submitModelToGPU(Model* model)
{
	// Check if the model is animated
	bool isAnimated = false;
	if (dynamic_cast<AnimatedModel*>(model) != nullptr)
	{
		isAnimated = true;

		// Submit the matrices to be uploaded everyframe
		CopyPerFrameTask* cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);

		AnimatedModel* aModel = static_cast<AnimatedModel*>(model);
		const ConstantBuffer* cb = aModel->GetConstantBuffer();

		const void* data = aModel->GetUploadMatrices()->data();
		std::tuple<Resource*, Resource*, const void*> matrices(
			cb->GetUploadResource(),
			cb->GetDefaultResource(),
			data);

		cpft->Submit(&matrices);
	}

	for (unsigned int i = 0; i < model->GetSize(); i++)
	{
		Mesh* mesh = model->GetMeshAt(i);

		// Submit more data if the model is animated
		if (isAnimated == true)
		{
			AnimatedMesh* am = static_cast<AnimatedMesh*>(mesh);

			CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);

			// Submit the basic vertex data again. These vertex data will remain unchange during animations,
			// while the other resource will contain the modified vertex data. But as the initial state, both resources
			// will contain the same data.
			std::tuple<Resource*, Resource*, const void*> defaultResourceOrigVertices(
				am->GetUploadResourceOrigVertices(),
				am->GetDefaultResourceOrigVertices(),
				mesh->m_Vertices.data());

			std::tuple<Resource*, Resource*, const void*> defaultResourceVertexWeights(
				am->GetUploadResourceVertexWeights(),
				am->GetDefaultResourceVertexWeights(),
				am->GetVertexWeights()->data());

			codt->Submit(&defaultResourceOrigVertices);
			codt->Submit(&defaultResourceVertexWeights);
		}

		// Submit Mesh
		submitMeshToCodt(mesh);

		Texture* texture;
		// Submit Material
		texture = model->GetMaterialAt(i)->GetTexture(TEXTURE2D_TYPE::ALBEDO);
		submitTextureToCodt(texture);
		texture = model->GetMaterialAt(i)->GetTexture(TEXTURE2D_TYPE::ROUGHNESS);
		submitTextureToCodt(texture);
		texture = model->GetMaterialAt(i)->GetTexture(TEXTURE2D_TYPE::METALLIC);
		submitTextureToCodt(texture);
		texture = model->GetMaterialAt(i)->GetTexture(TEXTURE2D_TYPE::NORMAL);
		submitTextureToCodt(texture);
		texture = model->GetMaterialAt(i)->GetTexture(TEXTURE2D_TYPE::EMISSIVE);
		submitTextureToCodt(texture);
		texture = model->GetMaterialAt(i)->GetTexture(TEXTURE2D_TYPE::OPACITY);
		submitTextureToCodt(texture);
	}
}

void Renderer::submitTextureToCodt(Texture* texture)
{
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
	codt->SubmitTexture(texture);
}

Entity* const Renderer::GetPickedEntity() const
{
	return m_pPickedEntity;
}

Scene* const Renderer::GetActiveScene() const
{
	return m_pCurrActiveScene;
}

const Window* const Renderer::GetWindow() const
{
	return m_pWindow;
}

void Renderer::setRenderTasksPrimaryCamera()
{
	m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::ANIMATION_DEPTH_PRE_PASS]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_CONSTANT]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_TEXTURE]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::OUTLINE]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::SKYBOX]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::PARTICLE]->SetCamera(m_pScenePrimaryCamera);

	if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
	{
		m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME]->SetCamera(m_pScenePrimaryCamera);
	}
}

bool Renderer::createDevice()
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
	
		// Check to see if the adapter supports Direct3D 12, but don't create the actual m_pDevice yet.
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
		if (SUCCEEDED(hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_pDevice5))))
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
		//Create warp m_pDevice if no adapter was found.
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice5));
	}

	SAFE_RELEASE(&factory);

	return deviceCreated;
}

void Renderer::createCommandQueues()
{
	// Direct
	D3D12_COMMAND_QUEUE_DESC cqdDirect = {};
	cqdDirect.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hr;
	hr = m_pDevice5->CreateCommandQueue(&cqdDirect, IID_PPV_ARGS(&m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]));
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Direct CommandQueue\n");
	}
	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->SetName(L"DirectQueue");

	// Compute
	D3D12_COMMAND_QUEUE_DESC cqdCompute = {};
	cqdCompute.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	hr = m_pDevice5->CreateCommandQueue(&cqdCompute, IID_PPV_ARGS(&m_CommandQueues[COMMAND_INTERFACE_TYPE::COMPUTE_TYPE]));
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Compute CommandQueue\n");
	}
	m_CommandQueues[COMMAND_INTERFACE_TYPE::COMPUTE_TYPE]->SetName(L"ComputeQueue");

	// Copy
	D3D12_COMMAND_QUEUE_DESC cqdCopy = {};
	cqdCopy.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	hr = m_pDevice5->CreateCommandQueue(&cqdCopy, IID_PPV_ARGS(&m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]));
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Copy CommandQueue\n");
	}
	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->SetName(L"CopyQueue");
}

void Renderer::createSwapChain()
{
	UINT resolutionWidth = std::atoi(Option::GetInstance().GetVariable("i_resolutionWidth").c_str());
	UINT resolutionHeight = std::atoi(Option::GetInstance().GetVariable("i_resolutionHeight").c_str());

	m_pSwapChain = new SwapChain(
		m_pDevice5,
		m_pWindow->GetHwnd(),
		resolutionWidth, resolutionHeight,
		m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
}

void Renderer::createMainDSV()
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	UINT resolutionWidth = 0;
	UINT resolutionHeight = 0;
	m_pSwapChain->GetDX12SwapChain()->GetSourceSize(&resolutionWidth, &resolutionHeight);

	m_pMainDepthStencil = new DepthStencil(
		m_pDevice5,
		resolutionWidth, resolutionHeight,
		L"MainDSV",
		&dsvDesc,
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV]);
}

void Renderer::createRootSignature()
{
	m_pRootSignature = new RootSignature(m_pDevice5);
}

void Renderer::createFullScreenQuad()
{
	std::vector<Vertex> vertexVector;
	std::vector<unsigned int> indexVector;

	Vertex vertices[4] = {};
	vertices[0].pos = { -1.0f, 1.0f, 1.0f };
	vertices[0].uv = { 0.0f, 0.0f, };

	vertices[1].pos = { -1.0f, -1.0f, 1.0f };
	vertices[1].uv = { 0.0f, 1.0f };

	vertices[2].pos = { 1.0f, 1.0f, 1.0f };
	vertices[2].uv = { 1.0f, 0.0f };

	vertices[3].pos = { 1.0f, -1.0f, 1.0f };
	vertices[3].uv = { 1.0f, 1.0f};

	for (unsigned int i = 0; i < 4; i++)
	{
		vertexVector.push_back(vertices[i]);
	}
	indexVector.push_back(1);
	indexVector.push_back(0);
	indexVector.push_back(3);
	indexVector.push_back(0);
	indexVector.push_back(2);
	indexVector.push_back(3);

	m_pFullScreenQuad = new Mesh(&vertexVector, &indexVector);

	// init dx12 resources
	m_pFullScreenQuad->Init(m_pDevice5, m_DescriptorHeaps.at(DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV));
}

void Renderer::updateMousePicker()
{
	m_pMousePicker->UpdateRay();

	component::BoundingBoxComponent* pickedBoundingBox = nullptr;

	float tempDist;
	float closestDist = MAXNUMBER;

	for (component::BoundingBoxComponent* bbc : m_BoundingBoxesToBePicked)
	{
		// Reset picked m_Entities from last frame
		bbc->IsPickedThisFrame() = false;

		for (unsigned int i = 0; i < bbc->GetNumBoundingBoxes(); i++)
		{
			if (m_pMousePicker->Pick(bbc, tempDist, i) == true)
			{
				if (tempDist < closestDist)
				{
					pickedBoundingBox = bbc;

					closestDist = tempDist;
				}
			}
		}
	}

	// If an object intersected with the ray
	if (closestDist < MAXNUMBER)
	{
		pickedBoundingBox->IsPickedThisFrame() = true;

		// Set the object to me drawn in outliningRenderTask
		Entity* parentOfPickedObject = pickedBoundingBox->GetParent();
		component::ModelComponent*		mc = parentOfPickedObject->GetComponent<component::ModelComponent>();
		component::TransformComponent*	tc = parentOfPickedObject->GetComponent<component::TransformComponent>();

		static_cast<OutliningRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::OUTLINE])->SetObjectToOutline(&std::make_pair(mc, tc));

		m_pPickedEntity = parentOfPickedObject;
	}
	else
	{
		// No object was picked, reset the outlingRenderTask
		static_cast<OutliningRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::OUTLINE])->Clear();
		m_pPickedEntity = nullptr;
	}
}

void Renderer::initRenderTasks()
{

#pragma region DepthPrePass

	/* Depth Pre-Pass rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdDepthPrePass = {};
	gpsdDepthPrePass.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// RenderTarget
	gpsdDepthPrePass.NumRenderTargets = 0;
	gpsdDepthPrePass.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	// Depthstencil usage
	gpsdDepthPrePass.SampleDesc.Count = 1;
	gpsdDepthPrePass.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdDepthPrePass.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdDepthPrePass.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdDepthPrePass.RasterizerState.DepthBias = 0;
	gpsdDepthPrePass.RasterizerState.DepthBiasClamp = 0.0f;
	gpsdDepthPrePass.RasterizerState.SlopeScaledDepthBias = 0.0f;
	gpsdDepthPrePass.RasterizerState.FrontCounterClockwise = false;

	// Specify Blend descriptions
	// copy of defaultRTdesc
	D3D12_RENDER_TARGET_BLEND_DESC depthPrePassRTdesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdDepthPrePass.BlendState.RenderTarget[i] = depthPrePassRTdesc;

	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC depthPrePassDsd = {};
	depthPrePassDsd.DepthEnable = true;
	depthPrePassDsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthPrePassDsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// DepthStencil
	depthPrePassDsd.StencilEnable = false;
	gpsdDepthPrePass.DepthStencilState = depthPrePassDsd;
	gpsdDepthPrePass.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdDepthPrePassVector;
	gpsdDepthPrePassVector.push_back(&gpsdDepthPrePass);

	RenderTask* DepthPrePassRenderTask = new DepthRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"DepthVertex.hlsl", L"DepthPixel.hlsl",
		&gpsdDepthPrePassVector,
		L"DepthPrePassPSO",
		FLAG_THREAD::RENDER);

	DepthPrePassRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	DepthPrePassRenderTask->SetSwapChain(m_pSwapChain);
	DepthPrePassRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion DepthPrePass

#pragma region AnimationPass
	/* Depth Pre-Pass rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdAnimatedDepthPrePass = {};
	gpsdAnimatedDepthPrePass.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// RenderTarget
	gpsdAnimatedDepthPrePass.NumRenderTargets = 0;
	gpsdAnimatedDepthPrePass.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	// Depthstencil usage
	gpsdAnimatedDepthPrePass.SampleDesc.Count = 1;
	gpsdAnimatedDepthPrePass.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdAnimatedDepthPrePass.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdAnimatedDepthPrePass.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdAnimatedDepthPrePass.RasterizerState.DepthBias = 0;
	gpsdAnimatedDepthPrePass.RasterizerState.DepthBiasClamp = 0.0f;
	gpsdAnimatedDepthPrePass.RasterizerState.SlopeScaledDepthBias = 0.0f;
	gpsdAnimatedDepthPrePass.RasterizerState.FrontCounterClockwise = false;

	// Specify Blend descriptions
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdAnimatedDepthPrePass.BlendState.RenderTarget[i] = depthPrePassRTdesc;

	gpsdDepthPrePass.DepthStencilState = depthPrePassDsd;
	gpsdDepthPrePass.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdDepthPreAnimationVector;
	gpsdDepthPreAnimationVector.push_back(&gpsdAnimatedDepthPrePass);

	RenderTask* animationDepthPreRenderTask = new AnimatedDepthRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"AnimationDepthVertex.hlsl", L"AnimationDepthPixel.hlsl",
		&gpsdDepthPreAnimationVector,
		L"animationDepthPrePSO",
		FLAG_THREAD::RENDER);

	animationDepthPreRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	animationDepthPreRenderTask->SetSwapChain(m_pSwapChain);
	animationDepthPreRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion AnimationPass

#pragma region ForwardRendering
	/* Forward rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdForwardRender = {};
	gpsdForwardRender.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdForwardRender.NumRenderTargets = 2;
	gpsdForwardRender.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	gpsdForwardRender.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
		D3D12_BLEND_ZERO, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdForwardRender.BlendState.RenderTarget[i] = defaultRTdesc;

	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencil
	dsd.StencilEnable = false;
	gpsdForwardRender.DepthStencilState = dsd;
	gpsdForwardRender.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	/* Forward rendering with stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdForwardRenderStencilTest = gpsdForwardRender;

	// Only change stencil testing
	dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencil
	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0x00;
	dsd.StencilWriteMask = 0xff;
	const D3D12_DEPTH_STENCILOP_DESC stencilWriteAllways =
	{
		D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_REPLACE,
		D3D12_COMPARISON_FUNC_ALWAYS
	};
	dsd.FrontFace = stencilWriteAllways;
	dsd.BackFace = stencilWriteAllways;

	gpsdForwardRenderStencilTest.DepthStencilState = dsd;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdForwardRenderVector;
	gpsdForwardRenderVector.push_back(&gpsdForwardRender);
	gpsdForwardRenderVector.push_back(&gpsdForwardRenderStencilTest);

	RenderTask* forwardRenderTask = new FowardRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"ForwardVertex.hlsl", L"ForwardPixel.hlsl",
		&gpsdForwardRenderVector,
		L"ForwardRenderingPSO",
		FLAG_THREAD::RENDER);

	forwardRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetDefaultResource());
	forwardRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetDefaultResource());
	forwardRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	forwardRenderTask->SetSwapChain(m_pSwapChain);
	forwardRenderTask->AddRenderTargetView("brightTarget", std::get<1>(*m_pBloomResources->GetBrightTuple()));
	forwardRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion ForwardRendering

#pragma region DownSampleTextureTask
	/* Forward rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdDownSampleTexture = {};
	gpsdDownSampleTexture.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdDownSampleTexture.NumRenderTargets = 1;
	gpsdDownSampleTexture.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

	// Depthstencil usage
	gpsdDownSampleTexture.SampleDesc.Count = 1;
	gpsdDownSampleTexture.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdDownSampleTexture.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdDownSampleTexture.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdDownSampleTexture.RasterizerState.FrontCounterClockwise = false;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdDownSampleTexture.BlendState.RenderTarget[i] = defaultRTdesc;

	// Depth descriptor
	dsd = {};
	dsd.DepthEnable = false;
	dsd.StencilEnable = false;
	gpsdDownSampleTexture.DepthStencilState = dsd;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdDownSampleTextureVector;
	gpsdDownSampleTextureVector.push_back(&gpsdDownSampleTexture);

	RenderTask* downSampleTask = new DownSampleRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"DownSampleVertex.hlsl", L"DownSamplePixel.hlsl",
		&gpsdDownSampleTextureVector,
		L"DownSampleTexturePSO",
		std::get<2>(*m_pBloomResources->GetBrightTuple()),		// Read from this in actual resolution
		m_pBloomResources->GetPingPongResource(0)->GetRTV(),	// Write to this in 1280x720
		FLAG_THREAD::RENDER);
	
	static_cast<DownSampleRenderTask*>(downSampleTask)->SetFullScreenQuad(m_pFullScreenQuad);
	downSampleTask->SetSwapChain(m_pSwapChain);
	downSampleTask->SetDescriptorHeaps(m_DescriptorHeaps);
	static_cast<DownSampleRenderTask*>(downSampleTask)->SetFullScreenQuadInSlotInfo();

#pragma endregion DownSampleTextureTask

#pragma region ModelOutlining
	/* Forward rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdModelOutlining = gpsdForwardRenderStencilTest;
	gpsdModelOutlining.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	dsd = {};
	dsd.DepthEnable = false;	// Maybe enable if we dont want the object to "highlight" through other objects
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// DepthStencil
	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xff;
	dsd.StencilWriteMask = 0x00;
	const D3D12_DEPTH_STENCILOP_DESC stencilNotEqual =
	{ 
		D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_REPLACE,
		D3D12_COMPARISON_FUNC_NOT_EQUAL
	};
	dsd.FrontFace = stencilNotEqual;
	dsd.BackFace = stencilNotEqual;

	gpsdModelOutlining.DepthStencilState = dsd;
	gpsdModelOutlining.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdOutliningVector;
	gpsdOutliningVector.push_back(&gpsdModelOutlining);

	RenderTask* outliningRenderTask = new OutliningRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"OutlinedVertex.hlsl", L"OutlinedPixel.hlsl",
		&gpsdOutliningVector,
		L"outliningScaledPSO",
		FLAG_THREAD::RENDER);
	
	outliningRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	outliningRenderTask->SetSwapChain(m_pSwapChain);
	outliningRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion ModelOutlining

#pragma region SkyboxRendering
	/* Forward rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdSkyboxRender = {};
	gpsdSkyboxRender.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdSkyboxRender.NumRenderTargets = 1;
	gpsdSkyboxRender.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	// Depthstencil usage
	gpsdSkyboxRender.SampleDesc.Count = 1;
	gpsdSkyboxRender.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdSkyboxRender.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdSkyboxRender.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	gpsdSkyboxRender.RasterizerState.FrontCounterClockwise = false;

	// Specify Blend descriptions
	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdSkyboxRender.BlendState.RenderTarget[i] = defaultRTdesc; // Defined in ForwardRendering

	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC dsdSkybox = {};
	dsdSkybox.DepthEnable = true;
	dsdSkybox.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	dsdSkybox.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencil
	dsdSkybox.StencilEnable = false;
	gpsdSkyboxRender.DepthStencilState = dsdSkybox;
	gpsdSkyboxRender.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdSkyboxRenderVector;
	gpsdSkyboxRenderVector.push_back(&gpsdSkyboxRender);

	RenderTask* skyboxRenderTask = new SkyboxRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"SkyboxVertex.hlsl", L"SkyboxPixel.hlsl",
		&gpsdSkyboxRenderVector,
		L"SkyboxRenderingPSO",
		FLAG_THREAD::RENDER);

	skyboxRenderTask->SetSwapChain(m_pSwapChain);
	skyboxRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	skyboxRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion SkyboxRendering

#pragma region Blend
	// ------------------------ TASK 2: BLEND ---------------------------- FRONTCULL

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdBlendFrontCull = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdBlendBackCull = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdParticleEffect = {};
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdBlendVector;
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdParticleVector;

	gpsdBlendFrontCull.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdBlendFrontCull.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
	{
		gpsdBlendFrontCull.BlendState.RenderTarget[i] = blendRTdesc;
	}

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
	gpsdBlendFrontCull.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	// ------------------------ TASK 2: BLEND ---------------------------- BACKCULL

	gpsdBlendBackCull.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdBlendBackCull.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
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


	// Particle Effect
	gpsdParticleEffect.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdParticleEffect.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	gpsdParticleEffect.NumRenderTargets = 1;
	// Depthstencil usage
	gpsdParticleEffect.SampleDesc.Count = 1;
	gpsdParticleEffect.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdParticleEffect.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdParticleEffect.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdParticleEffect.BlendState.RenderTarget[i] = blendRTdesc;

	// DepthStencil
	dsdBlend.StencilEnable = false;
	dsdBlend.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;


	// Push back to vector
	gpsdBlendVector.push_back(&gpsdBlendFrontCull);
	gpsdBlendVector.push_back(&gpsdBlendBackCull);


	gpsdParticleVector.push_back(&gpsdParticleEffect);

	RenderTask* transparentConstantRenderTask = new TransparentRenderTask(m_pDevice5,
		m_pRootSignature,
		L"TransparentConstantVertex.hlsl",
		L"TransparentConstantPixel.hlsl",
		&gpsdBlendVector,
		L"BlendPSOConstant",
		FLAG_THREAD::RENDER);

	transparentConstantRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetDefaultResource());
	transparentConstantRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetDefaultResource());
	transparentConstantRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	transparentConstantRenderTask->SetSwapChain(m_pSwapChain);
	transparentConstantRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);
	
	/*---------------------------------- TRANSPARENT_TEXTURE_RENDERTASK -------------------------------------*/
	RenderTask* transparentTextureRenderTask = new TransparentRenderTask(m_pDevice5,
		m_pRootSignature,
		L"TransparentTextureVertex.hlsl",
		L"TransparentTexturePixel.hlsl",
		&gpsdBlendVector,
		L"BlendPSOTexture",
		FLAG_THREAD::RENDER);

	transparentTextureRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetDefaultResource());
	transparentTextureRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetDefaultResource());
	transparentTextureRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	transparentTextureRenderTask->SetSwapChain(m_pSwapChain);
	transparentTextureRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);







	/*---------------------------------- PARTICLE_RENDERTASK -------------------------------------*/
	RenderTask* particleRenderTask = new ParticleRenderTask(m_pDevice5,
		m_pRootSignature,
		L"ParticleVertex.hlsl",
		L"ParticlePixel.hlsl",
		&gpsdParticleVector,
		L"ParticlePSO",
		FLAG_THREAD::RENDER);

	particleRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetDefaultResource());
	particleRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetDefaultResource());
	particleRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	particleRenderTask->SetSwapChain(m_pSwapChain);
	particleRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

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
	gpsdShadow.RasterizerState.DepthBias = 1000;
	gpsdShadow.RasterizerState.DepthBiasClamp = 0.0f;
	gpsdShadow.RasterizerState.SlopeScaledDepthBias = 3.0f;
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
		m_pDevice5,
		m_pRootSignature,
		L"DepthVertex.hlsl", L"DepthPixel.hlsl",
		&gpsdShadowVector,
		L"ShadowPSO",
		FLAG_THREAD::RENDER);

	shadowRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);
#pragma endregion ShadowPass

#pragma region WireFrame
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdWireFrame = {};
	gpsdWireFrame.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdWireFrame.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
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

	RenderTask* wireFrameRenderTask = new WireframeRenderTask(m_pDevice5,
		m_pRootSignature,
		L"WhiteVertex.hlsl", L"WhitePixel.hlsl",
		&gpsdWireFrameVector,
		L"WireFramePSO",
		FLAG_THREAD::RENDER);

	wireFrameRenderTask->SetSwapChain(m_pSwapChain);
	wireFrameRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);
#pragma endregion WireFrame

#pragma region MergePass
	/* Forward rendering without stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdMergePass = {};
	gpsdMergePass.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdMergePass.NumRenderTargets = 1;
	gpsdMergePass.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	// Depthstencil usage
	gpsdMergePass.SampleDesc.Count = 1;
	gpsdMergePass.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdMergePass.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdMergePass.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdMergePass.RasterizerState.FrontCounterClockwise = false;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsdMergePass.BlendState.RenderTarget[i] = blendRTdesc;

	// Depth descriptor
	D3D12_DEPTH_STENCIL_DESC dsdMergePass = {};
	dsdMergePass.DepthEnable = false;
	dsdMergePass.StencilEnable = false;
	gpsdMergePass.DepthStencilState = dsdMergePass;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdMergePassVector;
	gpsdMergePassVector.push_back(&gpsdMergePass);

	RenderTask* mergeTask = new MergeRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"MergeVertex.hlsl", L"MergePixel.hlsl",
		&gpsdMergePassVector,
		L"MergePassPSO",
		FLAG_THREAD::RENDER);

	static_cast<MergeRenderTask*>(mergeTask)->SetFullScreenQuad(m_pFullScreenQuad);
	static_cast<MergeRenderTask*>(mergeTask)->AddSRVIndexToMerge(m_pBloomResources->GetPingPongResource(0)->GetSRV()->GetDescriptorHeapIndex());
	mergeTask->SetSwapChain(m_pSwapChain);
	mergeTask->SetDescriptorHeaps(m_DescriptorHeaps);
	static_cast<MergeRenderTask*>(mergeTask)->CreateSlotInfo();
#pragma endregion MergePass

#pragma region Text 

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdText = {};

	gpsdText.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdText.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	gpsdText.NumRenderTargets = 1;
	// Depthstencil usage
	gpsdText.SampleDesc.Count = 1;
	gpsdText.SampleMask = UINT_MAX;
	// Rasterizer behaviour
	gpsdText.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdText.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdText.RasterizerState.FrontCounterClockwise = false;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		gpsdText.BlendState.RenderTarget[i] = blendRTdesc;
	}

	D3D12_DEPTH_STENCIL_DESC textDepthStencilDesc = {};
	textDepthStencilDesc.DepthEnable = false;
	gpsdText.DepthStencilState = textDepthStencilDesc;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdTextVector;
	gpsdTextVector.push_back(&gpsdText);

	RenderTask* textTask = new TextTask(
		m_pDevice5,
		m_pRootSignature,
		L"TextVertex.hlsl", L"TextPixel.hlsl",
		&gpsdTextVector,
		L"TextPSO",
		FLAG_THREAD::RENDER);

	textTask->SetSwapChain(m_pSwapChain);
	textTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion Text
	
#pragma region IMGUIRENDERTASK
	RenderTask* imGuiRenderTask = new ImGuiRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"", L"",
		nullptr,
		L"",
		FLAG_THREAD::RENDER);

	imGuiRenderTask->SetSwapChain(m_pSwapChain);
	imGuiRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion IMGUIRENDERTASK

#pragma region ComputeAndCopyTasks
	UINT resolutionWidth = 0;
	UINT resolutionHeight = 0;
	m_pSwapChain->GetDX12SwapChain()->GetSourceSize(&resolutionWidth, &resolutionHeight);

	// ComputeTasks
	std::vector<std::pair<std::wstring, std::wstring>> csNamePSOName;
	csNamePSOName.push_back(std::make_pair(L"ComputeBlurHorizontal.hlsl", L"blurHorizontalPSO"));
	csNamePSOName.push_back(std::make_pair(L"ComputeBlurVertical.hlsl", L"blurVerticalPSO"));
	ComputeTask* blurComputeTask = new BlurComputeTask(
		m_pDevice5, m_pRootSignature,
		csNamePSOName,
		COMMAND_INTERFACE_TYPE::DIRECT_TYPE,
		std::get<2>(*m_pBloomResources->GetBrightTuple()),
		m_pBloomResources->GetPingPongResource(0),
		m_pBloomResources->GetPingPongResource(1),
		m_pBloomResources->GetBlurWidth(), m_pBloomResources->GetBlurHeight(),
		FLAG_THREAD::RENDER);

	blurComputeTask->SetDescriptorHeaps(m_DescriptorHeaps);

	// CopyTasks
	CopyTask* copyPerFrameTask = new CopyPerFrameTask(m_pDevice5, COMMAND_INTERFACE_TYPE::DIRECT_TYPE, FLAG_THREAD::RENDER);
	CopyTask* copyOnDemandTask = new CopyOnDemandTask(m_pDevice5, COMMAND_INTERFACE_TYPE::DIRECT_TYPE, FLAG_THREAD::RENDER);

#pragma endregion ComputeAndCopyTasks
	
#pragma region Quad

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdQuad = {};

	gpsdQuad.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RenderTarget
	gpsdQuad.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	gpsdQuad.NumRenderTargets = 1;

	// Depthstencil usage
	gpsdQuad.SampleDesc.Count = 1;
	gpsdQuad.SampleMask = UINT_MAX;

	// Rasterizer behaviour
	gpsdQuad.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsdQuad.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpsdQuad.RasterizerState.FrontCounterClockwise = false;

	// Specify Blend descriptions
	D3D12_RENDER_TARGET_BLEND_DESC quadRTdesc{};
	quadRTdesc.BlendEnable = true;
	quadRTdesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	quadRTdesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	quadRTdesc.BlendOp = D3D12_BLEND_OP_ADD;
	quadRTdesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	quadRTdesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	quadRTdesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	quadRTdesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		gpsdQuad.BlendState.RenderTarget[i] = quadRTdesc;
	}

	D3D12_DEPTH_STENCIL_DESC quadDepthStencilDesc = {};
	quadDepthStencilDesc.DepthEnable = false;
	gpsdText.DepthStencilState = quadDepthStencilDesc;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdQuadVector;
	gpsdQuadVector.push_back(&gpsdQuad);

	RenderTask* quadTask = new QuadTask(
		m_pDevice5,
		m_pRootSignature,
		L"QuadVertex.hlsl", L"QuadPixel.hlsl",
		&gpsdQuadVector,
		L"QuadPSO",
		FLAG_THREAD::RENDER);

	quadTask->SetSwapChain(m_pSwapChain);
	quadTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion Quad

	// Add the tasks to desired vectors so they can be used in m_pRenderer
	/* -------------------------------------------------------------- */


	/* ------------------------- CopyQueue Tasks ------------------------ */

	m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME] = copyPerFrameTask;
	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND] = copyOnDemandTask;

	/* ------------------------- ComputeQueue Tasks ------------------------ */
	
	m_ComputeTasks[COMPUTE_TASK_TYPE::BLUR] = blurComputeTask;

	/* ------------------------- DirectQueue Tasks ---------------------- */
	m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS] = DepthPrePassRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::ANIMATION_DEPTH_PRE_PASS] = animationDepthPreRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW] = shadowRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER] = forwardRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_CONSTANT] = transparentConstantRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_TEXTURE] = transparentTextureRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME] = wireFrameRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::OUTLINE] = outliningRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::MERGE] = mergeTask;
	m_RenderTasks[RENDER_TASK_TYPE::TEXT] = textTask;
	m_RenderTasks[RENDER_TASK_TYPE::IMGUI] = imGuiRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::SKYBOX] = skyboxRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::DOWNSAMPLE] = downSampleTask;
	m_RenderTasks[RENDER_TASK_TYPE::QUAD] = quadTask;
	m_RenderTasks[RENDER_TASK_TYPE::PARTICLE] = particleRenderTask;

	// Pushback in the order of execution
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(copyOnDemandTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(copyPerFrameTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(DepthPrePassRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(animationDepthPreRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(shadowRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(forwardRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(downSampleTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(skyboxRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(transparentConstantRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(transparentTextureRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(particleRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(outliningRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			m_DirectCommandLists[i].push_back(wireFrameRenderTask->GetCommandInterface()->GetCommandList(i));
		}
	}

	// Compute shader to blur the RTV from forwardRenderTask
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(blurComputeTask->GetCommandInterface()->GetCommandList(i));
	}

	// Final pass (this pass will merge different textures together and put result in the swapchain backBuffer)
	// This will be used for pp-effects such as bloom.
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(mergeTask->GetCommandInterface()->GetCommandList(i));
	}

	// GUI
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(quadTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(textTask->GetCommandInterface()->GetCommandList(i));
	}

	if (DEVELOPERMODE_DEVINTERFACE == true)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			m_DirectCommandLists[i].push_back(imGuiRenderTask->GetCommandInterface()->GetCommandList(i));
		}
	}
}

void Renderer::setRenderTasksRenderComponents()
{
	m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::NO_DEPTH]);
	m_RenderTasks[RENDER_TASK_TYPE::ANIMATION_DEPTH_PRE_PASS]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_ANIMATED]);
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_OPAQUE]);
	m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_CONSTANT]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_CONSTANT]);
	m_RenderTasks[RENDER_TASK_TYPE::TRANSPARENT_TEXTURE]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE]);
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::GIVE_SHADOW]);
	m_RenderTasks[RENDER_TASK_TYPE::PARTICLE]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE]);

	static_cast<SkyboxRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SKYBOX])->SetSkybox(m_pSkyboxComponent);
}

void Renderer::createDescriptorHeaps()
{
	m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV] = new DescriptorHeap(m_pDevice5, DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV);
	m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV] = new DescriptorHeap(m_pDevice5, DESCRIPTOR_HEAP_TYPE::RTV);
	m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV] = new DescriptorHeap(m_pDevice5, DESCRIPTOR_HEAP_TYPE::DSV);
}

void Renderer::createFences()
{
	HRESULT hr = m_pDevice5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFenceFrame));

	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Fence\n");
	}
	m_FenceFrameValue = 1;

	// Event handle to use for GPU synchronization
	m_EventHandle = CreateEvent(0, false, false, 0);
}

void Renderer::waitForGPU()
{
	//Signal and increment the fence value.
	const UINT64 oldFenceValue = m_FenceFrameValue;
	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(m_pFenceFrame, oldFenceValue);
	m_FenceFrameValue++;

	//Wait until command queue is done.
	if (m_pFenceFrame->GetCompletedValue() < oldFenceValue)
	{
		m_pFenceFrame->SetEventOnCompletion(oldFenceValue, m_EventHandle);
		WaitForSingleObject(m_EventHandle, INFINITE);
	}
}

void Renderer::setRenderTasksGUI2DComponents()
{
	static_cast<QuadTask*>(m_RenderTasks[RENDER_TASK_TYPE::QUAD])->SetQuadComponents(&m_QuadComponents);
	static_cast<TextTask*>(m_RenderTasks[RENDER_TASK_TYPE::TEXT])->SetTextComponents(&m_TextComponents);
}

void Renderer::waitForFrame(unsigned int framesToBeAhead)
{
	static constexpr unsigned int nrOfFenceChangesPerFrame = 1;
	unsigned int fenceValuesToBeAhead = framesToBeAhead * nrOfFenceChangesPerFrame;

	//Wait if the CPU is "framesToBeAhead" number of frames ahead of the GPU
	if (m_pFenceFrame->GetCompletedValue() < m_FenceFrameValue - fenceValuesToBeAhead)
	{
		m_pFenceFrame->SetEventOnCompletion(m_FenceFrameValue - fenceValuesToBeAhead, m_EventHandle);
		WaitForSingleObject(m_EventHandle, INFINITE);
	}
}

// Saving these incase we for some reason want to go back
//void Renderer::waitForCopyOnDemand()
//{
//	//Signal and increment the fence value.
//	const UINT64 oldFenceValue = m_FenceFrameValue;
//	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->Signal(m_pFenceFrame, oldFenceValue);
//	m_FenceFrameValue++;
//
//	//Wait until command queue is done.
//	if (m_pFenceFrame->GetCompletedValue() < oldFenceValue)
//	{
//		m_pFenceFrame->SetEventOnCompletion(oldFenceValue, m_EventHandle);
//		WaitForSingleObject(m_EventHandle, INFINITE);
//	}
//}
//
//void Renderer::executeCopyOnDemand()
//{
//	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->SetCommandInterfaceIndex(0);
//	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Execute();
//	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(1, &m_CopyOnDemandCmdList[0]);
//	waitForCopyOnDemand();
//	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Clear();
//}

void Renderer::prepareScenes(std::vector<Scene*>* scenes)
{
	SubmitUploadPerFrameData();
	SubmitUploadPerSceneData();

	// -------------------- DEBUG STUFF --------------------
	// Test to change m_pCamera to the shadow casting m_lights cameras
	//auto& tuple = m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].at(0);
	//BaseCamera* tempCam = std::get<0>(tuple)->GetCamera();
	//m_pScenePrimaryCamera = tempCam;
	if (m_pScenePrimaryCamera == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "No primary camera was set in scenes\n");

		// Todo: Set default m_pCamera
	}
	if (m_pSkyboxComponent == nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Renderer::prepareScene: Skybox not found, please set one\n");
	}
	else
	{

	}
	m_pMousePicker->SetPrimaryCamera(m_pScenePrimaryCamera);

	setRenderTasksGUI2DComponents();
	setRenderTasksRenderComponents();
	setRenderTasksPrimaryCamera();
}

void Renderer::SubmitUploadPerSceneData()
{
	// ----- directional lights -----
	m_pCbPerSceneData->Num_Dir_Lights = m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].size();
	unsigned int index = 0;
	for (auto& tuple : m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT])
	{
		m_pCbPerSceneData->dirLightIndices[index].x = std::get<1>(tuple)->GetCBV()->GetDescriptorHeapIndex();
		index++;
	}
	// ----- directional m_lights -----

	// ----- point lights -----
	m_pCbPerSceneData->Num_Point_Lights = m_Lights[LIGHT_TYPE::POINT_LIGHT].size();
	index = 0;
	for (auto& tuple : m_Lights[LIGHT_TYPE::POINT_LIGHT])
	{
		m_pCbPerSceneData->pointLightIndices[index].x = std::get<1>(tuple)->GetCBV()->GetDescriptorHeapIndex();
		index++;
	}
	// ----- point m_lights -----

	// ----- spot lights -----
	m_pCbPerSceneData->Num_Spot_Lights = m_Lights[LIGHT_TYPE::SPOT_LIGHT].size();
	index = 0;
	for (auto& tuple : m_Lights[LIGHT_TYPE::SPOT_LIGHT])
	{
		m_pCbPerSceneData->spotLightIndices[index].x = std::get<1>(tuple)->GetCBV()->GetDescriptorHeapIndex();
		index++;
	}
	// ----- spot m_lights -----
	
	// Submit CB_PER_SCENE to be uploaded to VRAM
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
	const void* data = static_cast<const void*>(m_pCbPerSceneData);
	codt->Submit(&std::make_tuple(m_pCbPerScene->GetUploadResource(), m_pCbPerScene->GetDefaultResource(), data));

	// Submit static-light-data to be uploaded to VRAM
	ConstantBuffer* cb = nullptr;

	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		for (auto& tuple : m_Lights[type])
		{
			Light* light = std::get<0>(tuple);
			unsigned int lightFlags = light->GetLightFlags();
			if (lightFlags & FLAG_LIGHT::STATIC)
			{
				data = light->GetLightData();
				cb = std::get<1>(tuple);
				codt->Submit(&std::make_tuple(cb->GetUploadResource(), cb->GetDefaultResource(), data));
			}
		}
	}
}

void Renderer::SubmitUploadPerFrameData()
{
	// Submit dynamic-light-data to be uploaded to VRAM
	CopyPerFrameTask* cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
	const void* data = nullptr;
	ConstantBuffer* cb = nullptr;

	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		for (auto& tuple : m_Lights[type])
		{
			unsigned int lightFlags = static_cast<Light*>(std::get<0>(tuple))->GetLightFlags();
	
			if ((lightFlags & FLAG_LIGHT::STATIC) == 0)
			{
				data = std::get<0>(tuple)->GetLightData();
				cb = std::get<1>(tuple);
				cpft->Submit(&std::make_tuple(cb->GetUploadResource(), cb->GetDefaultResource(), data));
			}
		}
	}

	// CB_PER_FRAME_STRUCT
	if (cpft != nullptr)
	{
		data = static_cast<void*>(m_pCbPerFrameData);
		cpft->Submit(&std::tuple(m_pCbPerFrame->GetUploadResource(), m_pCbPerFrame->GetDefaultResource(), data));
	}
}

void Renderer::toggleFullscreen(WindowChange* evnt)
{
	m_FenceFrameValue++;
	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(m_pFenceFrame, m_FenceFrameValue);

	// Wait for all frames
	waitForFrame(0);

	// Wait for the threads which records the commandlists to complete
	m_pThreadPool->WaitForThreads(FLAG_THREAD::RENDER);

	for (auto task : m_RenderTasks)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			task->GetCommandInterface()->Reset(i);
		}
	}
	for (auto task : m_CopyTasks)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			task->GetCommandInterface()->Reset(i);
		}
	}
	for (auto task : m_ComputeTasks)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			task->GetCommandInterface()->Reset(i);
		}
	}

	m_pSwapChain->ToggleWindowMode(m_pDevice5,
		m_pWindow->GetHwnd(),
		m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

	// Change the member variables of the window class to match the swapchain
	UINT width = 0, height = 0;
	if (m_pSwapChain->IsFullscreen())
	{
		m_pSwapChain->GetDX12SwapChain()->GetSourceSize(&width, &height);
	}
	else
	{
		width = std::atoi(Option::GetInstance().GetVariable("i_windowWidth").c_str());
		height = std::atoi(Option::GetInstance().GetVariable("i_windowHeight").c_str());
	}

	Window* window = const_cast<Window*>(m_pWindow);
	window->SetScreenWidth(width);
	window->SetScreenHeight(height);

	for (auto task : m_RenderTasks)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			task->GetCommandInterface()->GetCommandList(i)->Close();
		}
	}
	for (auto task : m_CopyTasks)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			task->GetCommandInterface()->GetCommandList(i)->Close();
		}
	}
	for (auto task : m_ComputeTasks)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			task->GetCommandInterface()->GetCommandList(i)->Close();
		}
	}
}

SwapChain* Renderer::getSwapChain() const
{
	return m_pSwapChain;
}

void Renderer::submitTextToGPU(Text* text, TextManager* tm)
{
	// Submit to GPU
	const void* data = static_cast<const void*>(text->m_TextVertexVec.data());

	// Vertices
	Resource* uploadR = text->m_pUploadResourceVertices;
	Resource* defaultR = text->m_pDefaultResourceVertices;
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
	codt->Submit(&std::make_tuple(uploadR, defaultR, data));

	AssetLoader* al = AssetLoader::Get();
	bool isTextureOnGpu = al->IsFontTextureLoadedOnGPU(text->GetFont());

	if (isTextureOnGpu == false)
	{
		std::wstring fontPath = al->GetFontPath();
		std::wstring path = fontPath + text->GetFont()->GetName() + L".fnt";

		// Texture (only one per component)
		codt->SubmitTexture(tm->GetFontTexture());
		al->m_LoadedFonts[path].first = true;
	}
}
