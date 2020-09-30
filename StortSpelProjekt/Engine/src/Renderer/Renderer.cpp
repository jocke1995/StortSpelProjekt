#include "stdafx.h"
#include "Renderer.h"

// Misc
#include "../Misc/ThreadPool.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/Thread.h"
#include "../Misc/Window.h"
#include "../Misc/Option.h"

// ECS
#include "../ECS/Scene.h"
#include "../ECS/Entity.h"
#include "../ECS/Components/TextComponent.h"
#include "../ECS/Components/BoundingBoxComponent.h"

// Renderer-Engine 
#include "RootSignature.h"
#include "SwapChain.h"
#include "GPUMemory/DepthStencilView.h"
#include "ViewPool.h"
#include "BoundingBoxPool.h"
#include "CommandInterface.h"
#include "DescriptorHeap.h"
#include "Transform.h"
#include "BaseCamera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Text.h"

// GPUMemory
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
#include "DX12Tasks/WireframeRenderTask.h"
#include "DX12Tasks/OutliningRenderTask.h"
#include "DX12Tasks/ForwardRenderTask.h"
#include "DX12Tasks/BlendRenderTask.h"
#include "DX12Tasks/ShadowRenderTask.h"
#include "DX12Tasks/MergeRenderTask.h"
#include "DX12Tasks/TextTask.h"
#include "DX12Tasks/ImGuiRenderTask.h"

// Copy 
#include "DX12Tasks/CopyPerFrameTask.h"
#include "DX12Tasks/CopyOnDemandTask.h"

// Compute
#include "DX12Tasks/BlurComputeTask.h"

//ImGui
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "../ImGUI/imgui_impl_dx12.h"

#include "../ImGUI/ImGuiHandler.h"

Renderer::Renderer()
{
	m_RenderTasks.resize(RENDER_TASK_TYPE::NR_OF_RENDERTASKS);
	m_CopyTasks.resize(COPY_TASK_TYPE::NR_OF_COPYTASKS);
	m_ComputeTasks.resize(COMPUTE_TASK_TYPE::NR_OF_COMPUTETASKS);
}

Renderer::~Renderer()
{
	Log::Print("----------------------------  Deleting Renderer  ----------------------------------\n");
	waitForFrame(0);

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

	// Cleanup ImGui
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::InitD3D12(const Window *window, HINSTANCE hInstance, ThreadPool* threadPool)
{
	m_pThreadPool = threadPool;

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
	createSwapChain(window->GetHwnd());
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
	AssetLoader::Get(m_pDevice5, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV], window);

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
	ImGui_ImplWin32_Init(*window->GetHwnd());
	ImGui_ImplDX12_Init(m_pDevice5, NUM_SWAP_BUFFERS,
		DXGI_FORMAT_R16G16B16A16_FLOAT, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetID3D12DescriptorHeap(),
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetCPUHeapAt(imGuiTextureIndex),
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetGPUHeapAt(imGuiTextureIndex));

	

	initRenderTasks();

	// Submit the fullscreenQuad to be uploaded, but it won't be uploaded until a scene has been set to Draw
	const void* datavertices = m_pFullScreenQuad->m_Vertices.data();
	Resource* uplResourceVertices = m_pFullScreenQuad->m_pUploadResourceVertices;
	Resource* defResourceVertices = m_pFullScreenQuad->m_pDefaultResourceVertices;
	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Submit(&std::make_tuple(uplResourceVertices, defResourceVertices, datavertices));

	const void* dataindices = m_pFullScreenQuad->m_Indices.data();
	Resource* uplResourceindices = m_pFullScreenQuad->m_pUploadResourceIndices;
	Resource* defResourceindices = m_pFullScreenQuad->m_pDefaultResourceIndices;
	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]->Submit(&std::make_tuple(uplResourceindices, defResourceindices, dataindices));
}

void Renderer::Update(double dt)
{
	// Update scene
	m_pCurrActiveScene->Update(dt);
}

void Renderer::RenderUpdate(double dt)
{
	/* ------ ImGui ------*/
	if (DEVELOPERMODE_DEVINTERFACE == true)
	{
		ImGuiHandler::GetInstance().NewFrame();
	}
	// Update CB_PER_FRAME data
	m_pCbPerFrameData->camPos = m_pScenePrimaryCamera->GetPositionFloat3();

	// Picking
	updateMousePicker();

	// Update scene
	m_pCurrActiveScene->RenderUpdate(dt);

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
	
	// Update the entity-arrays inside the rendertasks
	setRenderTasksRenderComponents();
}

void Renderer::Execute(const HWND* hwnd)
{
	IDXGISwapChain4* dx12SwapChain = m_pSwapChain->GetDX12SwapChain();
	int backBufferIndex = dx12SwapChain->GetCurrentBackBufferIndex();
	int commandInterfaceIndex = m_FrameCounter++ % 2;

	static int a = 0;
	// Look if we toggle between fullscreen or windowed
	if (std::atoi(Option::GetInstance().GetVariable("b_fullscreen").c_str()) && a == 0)
	{
		a++;

		m_FenceFrameValue++;
		m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(m_pFenceFrame, m_FenceFrameValue);

		// Wait for all frames
		waitForFrame(0);

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

		m_pSwapChain->Toggle(m_pDevice5,
			hwnd,
			m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE],
			m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
			m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
		dx12SwapChain = m_pSwapChain->GetDX12SwapChain();
		backBufferIndex = dx12SwapChain->GetCurrentBackBufferIndex();

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

	CopyTask* copyTask = nullptr;
	ComputeTask* computeTask = nullptr;
	RenderTask* renderTask = nullptr;
	/* --------------------- Record command lists --------------------- */
	// Copy per frame
	copyTask = m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME];
	copyTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(copyTask, FLAG_THREAD::RENDER);

	// Recording shadowmaps
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::SHADOW];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);

	// Depth pre-pass
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS];
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);

	// Drawing
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);

	// Blending
	renderTask = m_RenderTasks[RENDER_TASK_TYPE::BLEND];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);

	// Blurring for bloom
	computeTask = m_ComputeTasks[COMPUTE_TASK_TYPE::BLUR];
	computeTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(computeTask, FLAG_THREAD::RENDER);

	// Outlining, if an object is picked
	m_RenderTasks[RENDER_TASK_TYPE::OUTLINE]->SetBackBufferIndex(backBufferIndex);
	m_RenderTasks[RENDER_TASK_TYPE::OUTLINE]->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(m_RenderTasks[RENDER_TASK_TYPE::OUTLINE], FLAG_THREAD::RENDER);

	renderTask = m_RenderTasks[RENDER_TASK_TYPE::TEXT];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);

	renderTask = m_RenderTasks[RENDER_TASK_TYPE::MERGE];
	renderTask->SetBackBufferIndex(backBufferIndex);
	renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);
	
	/* ----------------------------- DEVELOPERMODE CommandLists ----------------------------- */
	if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
	{
		renderTask = m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME];
		renderTask->SetBackBufferIndex(backBufferIndex);
		renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);
	}

	if (DEVELOPERMODE_DEVINTERFACE == true)
	{
		renderTask = m_RenderTasks[RENDER_TASK_TYPE::IMGUI];
		renderTask->SetBackBufferIndex(backBufferIndex);
		renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);
	}
	/* ----------------------------- DEVELOPERMODE CommandLists ----------------------------- */

	// Wait for the threads which records the commandlists to complete
	m_pThreadPool->WaitForThreads(FLAG_THREAD::RENDER | FLAG_THREAD::ALL);

	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->ExecuteCommandLists(
		m_DirectCommandLists[commandInterfaceIndex].size(), 
		m_DirectCommandLists[commandInterfaceIndex].data());

	/* --------------------------------------------------------------- */

	// Wait if the CPU is to far ahead of the gpu
	m_FenceFrameValue++;

	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(m_pFenceFrame, m_FenceFrameValue);
	waitForFrame();

	HRESULT hr = dx12SwapChain->Present(0, 0);
	
#ifdef _DEBUG
	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Swapchain Failed to present\n");
	}
#endif
}

Entity* const Renderer::GetPickedEntity() const
{
	return m_pPickedEntity;
}

Scene* const Renderer::GetActiveScene() const
{
	return m_pCurrActiveScene;
}

void Renderer::setRenderTasksPrimaryCamera()
{
	m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::BLEND]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW]->SetCamera(m_pScenePrimaryCamera);
	m_RenderTasks[RENDER_TASK_TYPE::OUTLINE]->SetCamera(m_pScenePrimaryCamera);

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

void Renderer::createSwapChain(const HWND *hwnd)
{
	UINT resolutionWidth = std::atoi(Option::GetInstance().GetVariable("i_resolutionWidth").c_str());
	UINT resolutionHeight = std::atoi(Option::GetInstance().GetVariable("i_resolutionHeight").c_str());

	m_pSwapChain = new SwapChain(
		m_pDevice5,
		hwnd,
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

	UINT resolutionWidth = std::atoi(Option::GetInstance().GetVariable("i_resolutionWidth").c_str());
	UINT resolutionHeight = std::atoi(Option::GetInstance().GetVariable("i_resolutionHeight").c_str());
	if (m_pSwapChain->IsFullscreen())
	{
		m_pSwapChain->GetDX12SwapChain()->GetSourceSize(&resolutionWidth, &resolutionHeight);
	}

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

	m_pFullScreenQuad = new Mesh(m_pDevice5, &vertexVector, &indexVector, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
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

		if (m_pMousePicker->Pick(bbc, tempDist) == true)
		{
			if (tempDist < closestDist)
			{
				pickedBoundingBox = bbc;

				closestDist = tempDist;
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
	// RenderTasks

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
		L"DepthPrePassPSO");

	
	// TODO: remove swapchain, using swapchains render view currently.
	DepthPrePassRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	DepthPrePassRenderTask->SetSwapChain(m_pSwapChain);
	DepthPrePassRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion DepthPrePass

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
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
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
		L"ForwardRenderingPSO");

	forwardRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetDefaultResource());
	forwardRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetDefaultResource());
	forwardRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	forwardRenderTask->SetSwapChain(m_pSwapChain);
	forwardRenderTask->AddRenderTarget("brightTarget", m_pBloomResources->GetRenderTargetView());
	forwardRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);


#pragma endregion ForwardRendering

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
		L"outliningScaledPSO");
	
	outliningRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	outliningRenderTask->SetSwapChain(m_pSwapChain);
	outliningRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion ModelOutlining

#pragma region Blend
	// ------------------------ TASK 2: BLEND ---------------------------- FRONTCULL

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdBlendFrontCull = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdBlendBackCull = {};
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdBlendVector;

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

	gpsdBlendBackCull.DepthStencilState = dsdBlend;
	gpsdBlendBackCull.DSVFormat = m_pMainDepthStencil->GetDSV()->GetDXGIFormat();

	gpsdBlendVector.push_back(&gpsdBlendFrontCull);
	gpsdBlendVector.push_back(&gpsdBlendBackCull);

	RenderTask* blendRenderTask = new BlendRenderTask(m_pDevice5,
		m_pRootSignature,
		L"BlendVertex.hlsl",
		L"BlendPixel.hlsl",
		&gpsdBlendVector,
		L"BlendPSO");

	blendRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetDefaultResource());
	blendRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetDefaultResource());
	blendRenderTask->SetMainDepthStencil(m_pMainDepthStencil);
	blendRenderTask->SetSwapChain(m_pSwapChain);
	blendRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);
	

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
		m_pDevice5,
		m_pRootSignature,
		L"DepthVertex.hlsl", L"DepthPixel.hlsl",
		&gpsdShadowVector,
		L"ShadowPSO");

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
		L"WireFramePSO");

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
		gpsdMergePass.BlendState.RenderTarget[i] = defaultRTdesc;

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
		L"MergePassPSO");

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

	D3D12_BLEND_DESC textBlendStateDesc = {};
	textBlendStateDesc.AlphaToCoverageEnable = FALSE;
	textBlendStateDesc.IndependentBlendEnable = FALSE;
	textBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;

	textBlendStateDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	textBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	textBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gpsdText.BlendState = textBlendStateDesc;
	gpsdText.NumRenderTargets = 1;

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
		L"TextPSO");

	textTask->SetSwapChain(m_pSwapChain);
	textTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion Text
	
#pragma region IMGUIRENDERTASK
	RenderTask* imGuiRenderTask = new ImGuiRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"", L"",
		nullptr,
		L"");

	imGuiRenderTask->SetSwapChain(m_pSwapChain);
	imGuiRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

#pragma endregion IMGUIRENDERTASK

	UINT resolutionWidth = std::atoi(Option::GetInstance().GetVariable("i_resolutionWidth").c_str());
	UINT resolutionHeight = std::atoi(Option::GetInstance().GetVariable("i_resolutionHeight").c_str());
	if (m_pSwapChain->IsFullscreen())
	{
		m_pSwapChain->GetDX12SwapChain()->GetSourceSize(&resolutionWidth, &resolutionHeight);
	}

	// ComputeTasks
	std::vector<std::pair<std::wstring, std::wstring>> csNamePSOName;
	csNamePSOName.push_back(std::make_pair(L"ComputeBlurHorizontal.hlsl", L"blurHorizontalPSO"));
	csNamePSOName.push_back(std::make_pair(L"ComputeBlurVertical.hlsl", L"blurVerticalPSO"));
	ComputeTask* blurComputeTask = new BlurComputeTask(
		m_pDevice5, m_pRootSignature,
		csNamePSOName,
		COMMAND_INTERFACE_TYPE::DIRECT_TYPE,
		m_pBloomResources->GetPingPongResource(0),
		m_pBloomResources->GetPingPongResource(1),
		resolutionWidth, resolutionHeight);

	blurComputeTask->SetDescriptorHeaps(m_DescriptorHeaps);

	// CopyTasks
	CopyTask* copyPerFrameTask = new CopyPerFrameTask(m_pDevice5, COMMAND_INTERFACE_TYPE::DIRECT_TYPE);
	CopyTask* copyOnDemandTask = new CopyOnDemandTask(m_pDevice5, COMMAND_INTERFACE_TYPE::COPY_TYPE);

	
	// Add the tasks to desired vectors so they can be used in m_pRenderer
	/* -------------------------------------------------------------- */


	/* ------------------------- CopyQueue Tasks ------------------------ */

	m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME] = copyPerFrameTask;
	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND] = copyOnDemandTask;

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_CopyOnDemandCmdList[i] = copyOnDemandTask->GetCommandInterface()->GetCommandList(i);
	}

	/* ------------------------- ComputeQueue Tasks ------------------------ */
	
	m_ComputeTasks[COMPUTE_TASK_TYPE::BLUR] = blurComputeTask;

	/* ------------------------- DirectQueue Tasks ---------------------- */
	m_RenderTasks[RENDER_TASK_TYPE::DEPTH_PRE_PASS] = DepthPrePassRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW] = shadowRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER] = forwardRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::BLEND] = blendRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME] = wireFrameRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::OUTLINE] = outliningRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::MERGE] = mergeTask;
	m_RenderTasks[RENDER_TASK_TYPE::TEXT] = textTask;
	m_RenderTasks[RENDER_TASK_TYPE::IMGUI] = imGuiRenderTask;

	// Pushback in the order of execution
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(copyPerFrameTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(shadowRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(DepthPrePassRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(forwardRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(blendRenderTask->GetCommandInterface()->GetCommandList(i));
	}

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_DirectCommandLists[i].push_back(textTask->GetCommandInterface()->GetCommandList(i));
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
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_OPAQUE]);
	m_RenderTasks[RENDER_TASK_TYPE::BLEND]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::DRAW_OPACITY]);
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW]->SetRenderComponents(&m_RenderComponents[FLAG_DRAW::GIVE_SHADOW]);
	static_cast<TextTask*>(m_RenderTasks[RENDER_TASK_TYPE::TEXT])->SetTextComponents(&m_TextComponents);
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

void Renderer::waitForCopyOnDemand()
{
	//Signal and increment the fence value.
	const UINT64 oldFenceValue = m_FenceFrameValue;
	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->Signal(m_pFenceFrame, oldFenceValue);
	m_FenceFrameValue++;

	//Wait until command queue is done.
	if (m_pFenceFrame->GetCompletedValue() < oldFenceValue)
	{
		m_pFenceFrame->SetEventOnCompletion(oldFenceValue, m_EventHandle);
		WaitForSingleObject(m_EventHandle, INFINITE);
	}		
}

void Renderer::removeComponents(Entity* entity)
{
	for (auto& renderComponents : m_RenderComponents)
	{
		for (int i = 0; i < renderComponents.second.size(); i++)
		{
			// Remove from all renderComponent-vectors if they are there
			Entity* parent = nullptr;
			parent = renderComponents.second[i].first->GetParent();
			if (parent == entity)
			{
				renderComponents.second.erase(renderComponents.second.begin() + i);
				setRenderTasksRenderComponents();
			}
		}
	}

	// Check if the entity is a textComponent
	for (int i = 0; i < m_TextComponents.size(); i++)
	{
		Entity* parent = m_TextComponents[i]->GetParent();
		if (parent == entity)
		{
			m_TextComponents.erase(m_TextComponents.begin() + i);
			setRenderTasksRenderComponents();
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

		for (auto& tuple : m_Lights[type])
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
				prepareCBPerScene();
				break;
			}
			j++;
		}
	}

	// Check if the entity got a boundingbox component.
	component::BoundingBoxComponent* bbc = entity->GetComponent<component::BoundingBoxComponent>();
	if (bbc != NULL)
	{
		if (bbc->GetParent() == entity)
		{
			// Stop drawing the wireFrame
			if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
			{
				static_cast<WireframeRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME])->ClearSpecific(bbc);
			}

			// Stop picking this boundingBox
			unsigned int i = 0;
			for (auto& bbcToBePicked : m_BoundingBoxesToBePicked)
			{
				if (bbcToBePicked == bbc)
				{
					m_BoundingBoxesToBePicked.erase(m_BoundingBoxesToBePicked.begin() + i);
					break;
				}
				i++;
			}
		}
	}
	return;
}

void Renderer::addComponents(Entity* entity)
{
	// Only add the m_Entities that actually should be drawn
	component::ModelComponent* mc = entity->GetComponent<component::ModelComponent>();
	if (mc != nullptr)
	{
		component::TransformComponent* tc = entity->GetComponent<component::TransformComponent>();
		if (tc != nullptr)
		{
			Mesh* mesh = mc->GetMeshAt(0);
			AssetLoader* al = AssetLoader::Get();
			std::wstring modelPath = *mesh->GetPath();
			bool isModelOnGpu = al->m_LoadedModels[modelPath].first;

			// If the model isn't on GPU, it will be uploaded below
			if (isModelOnGpu == false)
			{
				al->m_LoadedModels[modelPath].first = true;
			}

			// Submit Mesh/texture data to GPU if they haven't already been uploaded
			for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
			{
				mesh = mc->GetMeshAt(i);

				// Submit to the list which gets updated to the gpu each frame
				CopyPerFrameTask* cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);

				// Submit m_pMesh & texture Data to GPU if the data isn't already uploaded
				CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
				if (isModelOnGpu == false)
				{
					// Vertices
					const void* data = static_cast<const void*>(mesh->m_Vertices.data());
					Resource* uploadR = mesh->m_pUploadResourceVertices;
					Resource* defaultR = mesh->m_pDefaultResourceVertices;
					codt->Submit(&std::make_tuple(uploadR, defaultR, data));

					// inidices
					data = static_cast<const void*>(mesh->m_Indices.data());
					uploadR = mesh->m_pUploadResourceIndices;
					defaultR = mesh->m_pDefaultResourceIndices;
					codt->Submit(&std::make_tuple(uploadR, defaultR, data));
				}

				Material* meshMat = mc->GetMaterialAt(i);
				// Textures
				for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
				{
					TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
					Texture* texture = meshMat->GetTexture(type);

					// Check if the texture is on GPU before submitting to be uploaded
					if (al->m_LoadedTextures[texture->m_FilePath].first == false)
					{
						codt->SubmitTexture(texture);
						al->m_LoadedTextures[texture->m_FilePath].first = true;
					}
				}
			}

			// Finally store the object in the corresponding renderComponent vectors so it will be drawn
			if (FLAG_DRAW::DRAW_OPACITY & mc->GetDrawFlag())
			{
				m_RenderComponents[FLAG_DRAW::DRAW_OPACITY].push_back(std::make_pair(mc, tc));
			}

			if (FLAG_DRAW::DRAW_OPAQUE & mc->GetDrawFlag())
			{
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

	component::DirectionalLightComponent* dlc = entity->GetComponent<component::DirectionalLightComponent>();
	if (dlc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"DirectionalLight_DefaultResource";
		ConstantBuffer* cbd = m_pViewPool->GetFreeCBV(sizeof(DirectionalLight), resourceName);

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
			si = m_pViewPool->GetFreeShadowInfo(LIGHT_TYPE::DIRECTIONAL_LIGHT, resolution);
			static_cast<DirectionalLight*>(dlc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(dlc, si));
		}

		// Save in m_pRenderer
		m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].push_back(std::make_tuple(dlc, cbd, si));
	}

	// Currently no shadows are implemented for pointLights
	component::PointLightComponent* plc = entity->GetComponent<component::PointLightComponent>();
	if (plc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"PointLight_DefaultResource";
		ConstantBuffer* cbd = m_pViewPool->GetFreeCBV(sizeof(PointLight), resourceName);

		// Assign views required for shadows from the lightPool
		ShadowInfo* si = nullptr;

		// Save in m_pRenderer
		m_Lights[LIGHT_TYPE::POINT_LIGHT].push_back(std::make_tuple(plc, cbd, si));
	}

	component::SpotLightComponent* slc = entity->GetComponent<component::SpotLightComponent>();
	if (slc != nullptr)
	{
		// Assign CBV from the lightPool
		std::wstring resourceName = L"SpotLight_DefaultResource";
		ConstantBuffer* cbd = m_pViewPool->GetFreeCBV(sizeof(SpotLight), resourceName);

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
			si = m_pViewPool->GetFreeShadowInfo(LIGHT_TYPE::SPOT_LIGHT, resolution);
			static_cast<SpotLight*>(slc->GetLightData())->textureShadowMap = si->GetSRV()->GetDescriptorHeapIndex();

			ShadowRenderTask* srt = static_cast<ShadowRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::SHADOW]);
			srt->AddShadowCastingLight(std::make_pair(slc, si));
		}
		// Save in m_pRenderer
		m_Lights[LIGHT_TYPE::SPOT_LIGHT].push_back(std::make_tuple(slc, cbd, si));
	}

	component::CameraComponent* cc = entity->GetComponent<component::CameraComponent>();
	if (cc != nullptr)
	{
		if (cc->IsPrimary() == true)
		{
			m_pScenePrimaryCamera = cc->GetCamera();
		}
	}

	component::BoundingBoxComponent* bbc = entity->GetComponent<component::BoundingBoxComponent>();
	if (bbc != nullptr)
	{
		// Add it to m_pTask so it can be drawn
		if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
		{
			Mesh* m = BoundingBoxPool::Get()->CreateBoundingBoxMesh(bbc->GetPathOfModel());
			if (m == nullptr)
			{
				Log::PrintSeverity(Log::Severity::WARNING, "Forgot to initialize BoundingBoxComponent on Entity: %s\n", bbc->GetParent()->GetName().c_str());
				return;
			}

			// Submit to GPU
			CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
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

			static_cast<WireframeRenderTask*>(m_RenderTasks[RENDER_TASK_TYPE::WIREFRAME])->AddObjectToDraw(bbc);
		}

		// Add to vector so the mouse picker can check for intersections
		if (bbc->GetFlagOBB() & F_OBBFlags::PICKING)
		{
			m_BoundingBoxesToBePicked.push_back(bbc);
		}
	}

	component::TextComponent* textComp = entity->GetComponent<component::TextComponent>();
	if (textComp != nullptr)
	{
		std::map<std::string, TextData>* textDataMap = textComp->GetTextDataMap();

		for (auto textData : *textDataMap)
		{
			AssetLoader* al = AssetLoader::Get();
			int numOfCharacters = textComp->GetNumOfCharacters(textData.first);

			Text* text = new Text(m_pDevice5, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV], numOfCharacters, textComp->GetTexture());
			text->SetTextData(&textData.second, textComp->GetFont());
			
			textComp->SubmitText(text);

			// Look if data is already on the GPU

			CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);

			// Submit to GPU
			const void* data = static_cast<const void*>(text->m_TextVertexVec.data());

			// Vertices
			Resource* uploadR = text->m_pUploadResourceVertices;
			Resource* defaultR = text->m_pDefaultResourceVertices;
			codt->Submit(&std::make_tuple(uploadR, defaultR, data));

			// Texture
			codt->SubmitTexture(textComp->GetTexture());
		}

		// Finally store the text in m_pRenderer so it will be drawn
		m_TextComponents.push_back(textComp);
	}
}

void Renderer::prepareScene(Scene* scene)
{
	prepareCBPerFrame();
	prepareCBPerScene();

	// -------------------- DEBUG STUFF --------------------
	// Test to change m_pCamera to the shadow casting m_lights cameras
	// auto& tuple = m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].at(0);
	// BaseCamera* tempCam = std::get<0>(tuple)->GetCamera();
	// m_pScenePrimaryCamera = tempCam;
	if (m_pScenePrimaryCamera == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "No primary camera was set in scene: %s\n", scene->GetName());

		// Todo: Set default m_pCamera
	}

	m_pMousePicker->SetPrimaryCamera(m_pScenePrimaryCamera);
	scene->SetPrimaryCamera(m_pScenePrimaryCamera);
	setRenderTasksRenderComponents();
	setRenderTasksPrimaryCamera();

	m_pCurrActiveScene = scene;
}

void Renderer::prepareCBPerScene()
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

	// Upload CB_PER_SCENE to defaultheap
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
	const void* data = static_cast<const void*>(m_pCbPerSceneData);
	codt->Submit(&std::make_tuple(m_pCbPerScene->GetUploadResource(), m_pCbPerScene->GetDefaultResource(), data));
}

void Renderer::prepareCBPerFrame()
{
	CopyPerFrameTask* cpft = nullptr;
	const void* data = nullptr;
	ConstantBuffer* cbv = nullptr;

	// Lights
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		for (auto& tuple : m_Lights[type])
		{
			data = std::get<0>(tuple)->GetLightData();
			cbv = std::get<1>(tuple);

			cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
			cpft->Submit(&std::make_tuple(cbv->GetUploadResource(), cbv->GetDefaultResource(), data));
		}
	}

	// Materials are submitted in the copyPerFrameTask inside EditScene.
	// This was done so that a new entity (added during runetime) also would be added to the list.

	// CB_PER_FRAME_STRUCT
	if (cpft != nullptr)
	{
		data = static_cast<void*>(m_pCbPerFrameData);
		cpft->Submit(&std::tuple(m_pCbPerFrame->GetUploadResource(), m_pCbPerFrame->GetDefaultResource(), data));
	}
}
