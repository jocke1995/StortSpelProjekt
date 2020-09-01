#include "stdafx.h"
#include "Renderer.h"

// Misc
#include "../Misc/ThreadPool.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/Thread.h"
// ECS
#include "../ECS/Scene.h"
#include "../ECS/Entity.h"

// Renderer-Engine 
#include "RootSignature.h"
#include "SwapChain.h"
#include "DepthStencilView.h"
#include "ConstantBufferView.h"
#include "MousePicker.h"
#include "ViewPool.h"
#include "BoundingBoxPool.h"
#include "CommandInterface.h"
#include "DescriptorHeap.h"
#include "Transform.h"
#include "BaseCamera.h"

// Graphics
#include "DX12Tasks/WireframeRenderTask.h"
#include "DX12Tasks/OutliningRenderTask.h"
#include "DX12Tasks/ForwardRenderTask.h"
#include "DX12Tasks/BlendRenderTask.h"
#include "DX12Tasks/ShadowRenderTask.h"

// Copy 
#include "DX12Tasks/CopyPerFrameTask.h"
#include "DX12Tasks/CopyOnDemandTask.h"

// Compute
#include "DX12Tasks/ComputeTask.h"

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
	delete m_pSwapChain;
	delete m_pMainDSV;

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

	delete m_pWireFrameTask;
	delete m_pOutliningRenderTask;

	SAFE_RELEASE(&m_pDevice5);

	delete m_pMousePicker;

	delete m_pViewPool;
	delete m_pCbPerScene;
	delete m_pCbPerSceneData;
	delete m_pCbPerFrame;
	delete m_pCbPerFrameData;
}

void Renderer::InitD3D12(const HWND *hwnd, HINSTANCE hInstance, ThreadPool* threadPool)
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

	// Create SwapChain
	createSwapChain(hwnd);

	// Create Main DepthBuffer
	createMainDSV(hwnd);

	// Picking
	m_pMousePicker = new MousePicker();
	
	// Create Rootsignature
	createRootSignature();

	// Init Assetloader
	AssetLoader::Get(m_pDevice5, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

	// Init BoundingBoxPool
	BoundingBoxPool::Get(m_pDevice5, m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
	
	// Pool to handle GPU memory for the lights
	m_pViewPool = new ViewPool(
		m_pDevice5,
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV]);

	// Allocate memory for cbPerScene
	m_pCbPerScene = new ConstantBufferView(
		m_pDevice5, 
		sizeof(CB_PER_SCENE_STRUCT),
		L"CB_PER_SCENE_DEFAULT",
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetNextDescriptorHeapIndex(1),
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]
		);
	
	m_pCbPerSceneData = new CB_PER_SCENE_STRUCT();

	// Allocate memory for cbPerFrame
	m_pCbPerFrame = new ConstantBufferView(
		m_pDevice5,
		sizeof(CB_PER_FRAME_STRUCT),
		L"CB_PER_FRAME_DEFAULT",
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]->GetNextDescriptorHeapIndex(1),
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]
	);

	m_pCbPerFrameData = new CB_PER_FRAME_STRUCT();

	initRenderTasks();
}

void Renderer::Update(double dt)
{
	// Update CB_PER_FRAME data
	m_pCbPerFrameData->camPos = m_pScenePrimaryCamera->GetPositionFloat3();

	// Picking
	updateMousePicker();
	
	// Update scene
	m_pCurrActiveScene->UpdateScene(dt);
}

void Renderer::SortObjectsByDistance()
{
	struct DistFromCamera
	{
		double distance;
		component::MeshComponent* mc;
		component::TransformComponent* tc;
	};

	int numRenderComponents = m_RenderComponents.size();

	DistFromCamera* distFromCamArr = new DistFromCamera[numRenderComponents];

	// Get all the distances of each objects and store them by ID and distance
	DirectX::XMFLOAT3 camPos = m_pScenePrimaryCamera->GetPosition();
	for (int i = 0; i < numRenderComponents; i++)
	{
		DirectX::XMFLOAT3 objectPos = m_RenderComponents.at(i).second->GetTransform()->GetPositionXMFLOAT3();

		double distance = sqrt(	pow(camPos.x - objectPos.x, 2) +
								pow(camPos.y - objectPos.y, 2) +
								pow(camPos.z - objectPos.z, 2));

		// Save the object alongside its distance to the m_pCamera
		distFromCamArr[i].distance = distance;
		distFromCamArr[i].mc = m_RenderComponents.at(i).first;
		distFromCamArr[i].tc = m_RenderComponents.at(i).second;
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
	m_RenderComponents.clear();
	for (int i = 0; i < numRenderComponents; i++)
	{
		m_RenderComponents.push_back(std::make_pair(distFromCamArr[i].mc, distFromCamArr[i].tc));
	}

	// Free memory
	delete distFromCamArr;

	// Update the entity-arrays inside the rendertasks
	setRenderTasksRenderComponents();
}

void Renderer::Execute()
{
	IDXGISwapChain4* dx12SwapChain = m_pSwapChain->GetDX12SwapChain();
	int backBufferIndex = dx12SwapChain->GetCurrentBackBufferIndex();
	int commandInterfaceIndex = m_FrameCounter++ % 2;

	/* --------------------- Record copy command lists --------------------- */
	// Copy per frame
	CopyTask* ct = m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME];
	ct->SetCommandInterfaceIndex(commandInterfaceIndex);
	//threadpool->AddTask(ct, THREAD_FLAG::COPY_DATA);
	ct->Execute();
	//threadpool->WaitForThreads(THREAD_FLAG::COPY_DATA);


	/* --------------------- Execute copy command lists --------------------- */
	// Copy per frame
	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->ExecuteCommandLists(
		1,
		&m_CopyPerFrameCmdList[commandInterfaceIndex]);
	UINT64 copyFenceValue = ++m_FenceFrameValue;
	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->Signal(m_pFenceFrame, copyFenceValue);

	/* --------------------- Record direct commandlists --------------------- */
	for (RenderTask* renderTask : m_RenderTasks)
	{
		renderTask->SetBackBufferIndex(backBufferIndex);
		renderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		m_pThreadPool->AddTask(renderTask, FLAG_THREAD::RENDER);
		//renderTask->Execute();	// NON-MULTITHREADED-VERSION 
	}

	// Outlining, if an object is picked
	m_pOutliningRenderTask->SetBackBufferIndex(backBufferIndex);
	m_pOutliningRenderTask->SetCommandInterfaceIndex(commandInterfaceIndex);
	m_pThreadPool->AddTask(m_pOutliningRenderTask, FLAG_THREAD::RENDER);
	
	if (DRAWBOUNDINGBOX == true)
	{
		m_pWireFrameTask->SetBackBufferIndex(backBufferIndex);
		m_pWireFrameTask->SetCommandInterfaceIndex(commandInterfaceIndex);
		m_pThreadPool->AddTask(m_pWireFrameTask, FLAG_THREAD::RENDER);
	}
	
	// Wait for the threads which records the commandlists to complete
	m_pThreadPool->WaitForThreads(FLAG_THREAD::RENDER | FLAG_THREAD::ALL);
	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Wait(m_pFenceFrame, copyFenceValue);

	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->ExecuteCommandLists(
		m_DirectCommandLists[commandInterfaceIndex].size(), 
		m_DirectCommandLists[commandInterfaceIndex].data());

	/* --------------------------------------------------------------- */

	// Wait if the CPU is to far ahead of the gpu
	m_FenceFrameValue++;

	m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE]->Signal(m_pFenceFrame, m_FenceFrameValue);
	m_CommandQueues[COMMAND_INTERFACE_TYPE::COPY_TYPE]->Wait(m_pFenceFrame, m_FenceFrameValue);
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
	for (RenderTask* renderTask : m_RenderTasks)
	{
		renderTask->SetCamera(m_pScenePrimaryCamera);
	}

	m_pOutliningRenderTask->SetCamera(m_pScenePrimaryCamera);

	if (DRAWBOUNDINGBOX == true)
	{
		m_pWireFrameTask->SetCamera(m_pScenePrimaryCamera);
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
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	m_pSwapChain = new SwapChain(
		m_pDevice5,
		hwnd,
		width, height,
		m_CommandQueues[COMMAND_INTERFACE_TYPE::DIRECT_TYPE],
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV]);
}

void Renderer::createMainDSV(const HWND* hwnd)
{
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	m_pMainDSV = new DepthStencilView(
		m_pDevice5,
		width, height,	// width, height
		L"MainDSV_DEFAULT_RESOURCE",
		m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV],
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
}

void Renderer::createRootSignature()
{
	m_pRootSignature = new RootSignature(m_pDevice5);
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
		component::MeshComponent*		mc = parentOfPickedObject->GetComponent<component::MeshComponent>();
		component::TransformComponent*	tc = parentOfPickedObject->GetComponent<component::TransformComponent>();

		m_pOutliningRenderTask->SetObjectToOutline(&std::make_pair(mc, tc));

		m_pPickedEntity = parentOfPickedObject;
	}
	else
	{
		// No object was picked, reset the outlingRenderTask
		m_pOutliningRenderTask->Clear();
		m_pPickedEntity = nullptr;
	}
}

void Renderer::initRenderTasks()
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

	// DepthStencil
	dsd.StencilEnable = false;
	gpsdForwardRender.DepthStencilState = dsd;
	gpsdForwardRender.DSVFormat = m_pMainDSV->GetDXGIFormat();

	/* Forward rendering with stencil testing */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdForwardRenderStencilTest = gpsdForwardRender;

	// Only change stencil testing
	dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

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

	forwardRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetCBVResource());
	forwardRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetCBVResource());
	forwardRenderTask->AddRenderTarget("swapChain", m_pSwapChain);
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
	gpsdModelOutlining.DSVFormat = m_pMainDSV->GetDXGIFormat();

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> gpsdOutliningVector;
	gpsdOutliningVector.push_back(&gpsdModelOutlining);

	m_pOutliningRenderTask = new OutliningRenderTask(
		m_pDevice5,
		m_pRootSignature,
		L"OutlinedVertex.hlsl", L"OutlinedPixel.hlsl",
		&gpsdOutliningVector,
		L"outliningScaledPSO");
	
	m_pOutliningRenderTask->AddRenderTarget("swapChain", m_pSwapChain);
	m_pOutliningRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);

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
	gpsdBlendFrontCull.DSVFormat = m_pMainDSV->GetDXGIFormat();

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
	gpsdBlendBackCull.DSVFormat = m_pMainDSV->GetDXGIFormat();

	gpsdBlendVector.push_back(&gpsdBlendFrontCull);
	gpsdBlendVector.push_back(&gpsdBlendBackCull);

	RenderTask* blendRenderTask = new BlendRenderTask(m_pDevice5,
		m_pRootSignature,
		L"BlendVertex.hlsl",
		L"BlendPixel.hlsl",
		&gpsdBlendVector,
		L"BlendPSO");

	blendRenderTask->AddResource("cbPerFrame", m_pCbPerFrame->GetCBVResource());
	blendRenderTask->AddResource("cbPerScene", m_pCbPerScene->GetCBVResource());
	blendRenderTask->AddRenderTarget("swapChain", m_pSwapChain);
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
		L"ShadowVertex.hlsl", L"ShadowPixel.hlsl",
		&gpsdShadowVector,
		L"ShadowPSO");

	shadowRenderTask->SetDescriptorHeaps(m_DescriptorHeaps);
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

	m_pWireFrameTask = new WireframeRenderTask(m_pDevice5,
		m_pRootSignature,
		L"WhiteVertex.hlsl", L"WhitePixel.hlsl",
		&gpsdWireFrameVector,
		L"WireFramePSO");

	m_pWireFrameTask->AddRenderTarget("swapChain", m_pSwapChain);
	m_pWireFrameTask->SetDescriptorHeaps(m_DescriptorHeaps);
#pragma endregion WireFrame

	CopyTask* copyPerFrameTask = new CopyPerFrameTask(m_pDevice5);
	CopyTask* copyOnDemandTask = new CopyOnDemandTask(m_pDevice5);

	// Add the tasks to desired vectors so they can be used in m_pRenderer
	/* -------------------------------------------------------------- */


	/* ------------------------- CopyQueue Tasks ------------------------ */

	m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME] = copyPerFrameTask;
	m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND] = copyOnDemandTask;

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		m_CopyPerFrameCmdList[i] = copyPerFrameTask->GetCommandList(i);

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		m_CopyOnDemandCmdList[i] = copyOnDemandTask->GetCommandList(i);

	/* ------------------------- ComputeQueue Tasks ------------------------ */

	// None atm

	/* ------------------------- DirectQueue Tasks ---------------------- */
	m_RenderTasks[RENDER_TASK_TYPE::SHADOW] = shadowRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::FORWARD_RENDER] = forwardRenderTask;
	m_RenderTasks[RENDER_TASK_TYPE::BLEND] = blendRenderTask;

	// Pushback in the order of execution
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		m_DirectCommandLists[i].push_back(shadowRenderTask->GetCommandList(i));

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		m_DirectCommandLists[i].push_back(forwardRenderTask->GetCommandList(i));

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		m_DirectCommandLists[i].push_back(m_pOutliningRenderTask->GetCommandList(i));

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		m_DirectCommandLists[i].push_back(blendRenderTask->GetCommandList(i));

	if (DRAWBOUNDINGBOX == true)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
			m_DirectCommandLists[i].push_back(m_pWireFrameTask->GetCommandList(i));
	}
}

void Renderer::setRenderTasksRenderComponents()
{
	for (RenderTask* rendertask : m_RenderTasks)
	{
		rendertask->SetRenderComponents(&m_RenderComponents);
	}
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
	static constexpr unsigned int nrOfFenceChangesPerFrame = 2;
	unsigned int fenceValuesToBeAhead = framesToBeAhead * nrOfFenceChangesPerFrame;

	//Wait if the CPU is "framesToBeAhead" number of frames ahead of the GPU
	if (m_pFenceFrame->GetCompletedValue() < m_FenceFrameValue - fenceValuesToBeAhead)
	{
		m_pFenceFrame->SetEventOnCompletion(m_FenceFrameValue - fenceValuesToBeAhead, m_EventHandle);
		WaitForSingleObject(m_EventHandle, INFINITE);
	}
}

void Renderer::prepareCBPerScene()
{
	// ----- directional lights -----
	m_pCbPerSceneData->Num_Dir_Lights = m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT].size();
	unsigned int index = 0;
	for (auto& tuple : m_Lights[LIGHT_TYPE::DIRECTIONAL_LIGHT])
	{
		m_pCbPerSceneData->dirLightIndices[index].x = std::get<1>(tuple)->GetDescriptorHeapIndex();
		index++;
	}
	// ----- directional m_lights -----

	// ----- point lights -----
	m_pCbPerSceneData->Num_Point_Lights = m_Lights[LIGHT_TYPE::POINT_LIGHT].size();
	index = 0;
	for (auto& tuple : m_Lights[LIGHT_TYPE::POINT_LIGHT])
	{
		m_pCbPerSceneData->pointLightIndices[index].x = std::get<1>(tuple)->GetDescriptorHeapIndex();
		index++;
	}
	// ----- point m_lights -----

	// ----- spot lights -----
	m_pCbPerSceneData->Num_Spot_Lights = m_Lights[LIGHT_TYPE::SPOT_LIGHT].size();
	index = 0;
	for (auto& tuple : m_Lights[LIGHT_TYPE::SPOT_LIGHT])
	{
		m_pCbPerSceneData->spotLightIndices[index].x = std::get<1>(tuple)->GetDescriptorHeapIndex();
		index++;
	}
	// ----- spot m_lights -----

	// Upload CB_PER_SCENE to defaultheap
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND]);
	const void* data = static_cast<const void*>(m_pCbPerSceneData);
	codt->Submit(&std::make_tuple(m_pCbPerScene->GetUploadResource(), m_pCbPerScene->GetCBVResource(), data));
}

void Renderer::prepareCBPerFrame()
{
	CopyPerFrameTask* cpft = nullptr;
	const void* data = nullptr;
	ConstantBufferView* cbv = nullptr;

	// Lights
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		for (auto& tuple : m_Lights[type])
		{
			data = std::get<0>(tuple)->GetLightData();
			cbv = std::get<1>(tuple);

			cpft = static_cast<CopyPerFrameTask*>(m_CopyTasks[COPY_TASK_TYPE::COPY_PER_FRAME]);
			cpft->Submit(&std::make_tuple(cbv->GetUploadResource(), cbv->GetCBVResource(), data));
		}
	}

	// Materials are submitted in the copyPerFrameTask inside EditScene.
	// This was done so that a new entity (added during runetime) also would be added to the list.

	// CB_PER_FRAME_STRUCT
	if (cpft != nullptr)
	{
		data = static_cast<void*>(m_pCbPerFrameData);
		cpft->Submit(&std::tuple(m_pCbPerFrame->GetUploadResource(), m_pCbPerFrame->GetCBVResource(), data));
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
