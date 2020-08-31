#ifndef RENDERER_H
#define RENDERER_H

#include "RootSignature.h"
#include "SwapChain.h"
#include "DepthStencilView.h"
#include "MousePicker.h"

#include "ViewPool.h"
#include "BoundingBoxPool.h"

#include "../Misc/ThreadPool.h"

// ECS
#include "../ECS/Scene.h"
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/BoundingBoxComponent.h"

// Graphics
#include "DX12Tasks/WireframeRenderTask.h"
#include "DX12Tasks/OutliningRenderTask.h"
#include "DX12Tasks/ForwardRenderTask.h"
#include "DX12Tasks/BlendRenderTask.h"
#include "DX12Tasks/ShadowRenderTask.h"

// Copy
#include "DX12Tasks/CopyPerFrameTask.h"
#include "DX12Tasks/CopyOnDemandTask.h"

// Compute (Later include the specific tasks instead of this)
#include "DX12Tasks/ComputeTask.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	// Call once
	void InitD3D12(const HWND *hwnd, HINSTANCE hInstance, ThreadPool* threadPool);

	std::vector<Mesh*>* LoadModel(std::wstring path);
	Texture* LoadTexture(std::wstring path);

	// Call each frame
	void Update(double dt);
	void SortObjectsByDistance();
	void Execute();

	// PickedEntity
	Entity* const GetPickedEntity() const;

	// Scene
	Scene* const GetActiveScene() const;

private:
	friend class SceneManager;
	ThreadPool* threadPool = nullptr;

	// Camera
	BaseCamera* ScenePrimaryCamera = nullptr;
	void SetRenderTasksPrimaryCamera();

	unsigned int frameCounter = 0;

	// Device
	ID3D12Device5* device5 = nullptr;
	bool CreateDevice();

	// CommandQueues
	std::map<COMMAND_INTERFACE_TYPE, ID3D12CommandQueue*> commandQueues;
	void CreateCommandQueues();

	// Swapchain (inheriting from 'RenderTarget')
	SwapChain* swapChain = nullptr;
	void CreateSwapChain(const HWND *hwnd);

	// Depthbuffer
	DepthStencilView* mainDSV = nullptr;
	void CreateMainDSV(const HWND* hwnd);

	// Rootsignature
	RootSignature* rootSignature = nullptr;
	void CreateRootSignature();

	// Picking
	MousePicker* mousePicker = nullptr;
	Entity* pickedEntity = nullptr;
	void UpdateMousePicker();

	// Tasks
	std::vector<ComputeTask*> computeTasks;
	std::vector<CopyTask*>    copyTasks;
	std::vector<RenderTask*>  renderTasks;

	// Since these tasks wont operate on all objects, they will not be set in the same map as the other "rendertasks".
	WireframeRenderTask* wireFrameTask = nullptr;
	OutliningRenderTask* outliningRenderTask = nullptr;	
	void InitRenderTasks();

	// Group of components that's needed for rendering:
	std::vector<std::pair<component::MeshComponent*, component::TransformComponent*>> renderComponents;
	void SetRenderTasksRenderComponents();

	// BoundingBoxes to be picked
	std::vector<component::BoundingBoxComponent*> boundingBoxesToBePicked;

	ViewPool* viewPool = nullptr;
	std::map<LIGHT_TYPE, std::vector<std::tuple<Light*, ConstantBufferView*, ShadowInfo*>>> lights;

	// Current scene to be drawn
	Scene* currActiveScene = nullptr;
	ConstantBufferView* cbPerScene = nullptr;

	// update per frame
	CB_PER_FRAME_STRUCT* cbPerFrameData = nullptr;
	ConstantBufferView* cbPerFrame = nullptr;

	// Commandlists holders
	std::vector<ID3D12CommandList*> directCommandLists[NUM_SWAP_BUFFERS];
	std::vector<ID3D12CommandList*> computeCommandLists[NUM_SWAP_BUFFERS];
	ID3D12CommandList* m_CopyPerFrameCmdList[NUM_SWAP_BUFFERS];
	ID3D12CommandList* m_CopyOnDemandCmdList[NUM_SWAP_BUFFERS];
	
	// DescriptorHeaps
	std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> descriptorHeaps = {};
	void CreateDescriptorHeaps();

	// Fences
	HANDLE eventHandle = nullptr;
	ID3D12Fence1* fenceFrame = nullptr;
	UINT64 fenceFrameValue = 0;
	void CreateFences();

	void WaitForFrame(unsigned int framesToBeAhead = NUM_SWAP_BUFFERS - 1);

	// Setup Per-scene data and send to GPU
	void PrepareCBPerScene();
	// Submit per-frame data to the copyQueue that updates each frame
	void PrepareCBPerFrame();

	// Temporary.. these functions and variables are used to copy data to GPU on initialization
	void WaitForGpu();
};

#endif