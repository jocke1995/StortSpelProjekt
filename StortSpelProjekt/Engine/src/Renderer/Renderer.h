#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <map>

#include "structs.h"
#include "core.h"

// Misc
class ThreadPool;
class Window;

// Renderer Engine
class RootSignature;
class SwapChain;
class RenderTargetView;
class ViewPool;
class BoundingBoxPool;
class DescriptorHeap;
class Mesh;

// Views

// GPU Resources
class ConstantBuffer;
class ShaderResource;
class UnorderedAccess;
class DepthStencil;

// Enums
enum COMMAND_INTERFACE_TYPE;
enum class DESCRIPTOR_HEAP_TYPE;

// techniques
class ShadowInfo;
class MousePicker;
class Bloom;

// ECS
class Scene;
class Light;

// Graphics
class RenderTask;
class WireframeRenderTask;
class OutliningRenderTask;
class BaseCamera;

// Copy
class CopyTask;

// Compute
class ComputeTask;

// DX12 Forward Declarations
struct ID3D12CommandQueue;
struct ID3D12CommandList;
struct ID3D12Fence1;
struct ID3D12Device5;

// ECS
class Entity;
namespace component
{
	class ModelComponent;
	class TransformComponent;
	class BoundingBoxComponent;
	class TextComponent;
}

class Renderer
{
public:
	//Renderer();
	static Renderer& GetInstance();
	virtual ~Renderer();
	// For control of safe release of DirectX resources
	void DeleteDxResources();

	// PickedEntity
	Entity* const GetPickedEntity() const;
	// Scene
	Scene* const GetActiveScene() const;

	// Call once
	void InitD3D12(const Window* window, HINSTANCE hInstance, ThreadPool* threadPool);

	// Call on logic update *This should be moved to a more relevant logic class
	void Update(double dt);
	// Call each frame
	void RenderUpdate(double dt); //Please rename if logic update is removed
	void SortObjects();
	void Execute();

	// Render inits, these functions are called by respective components through SetScene to prepare 
	void InitComponent(std::string componentType);
	void InitModelComponent(Entity* entity);
	void InitDirectionalLightComponent(Entity* entity);
	void InitPointLightComponent(Entity* entity);
	void InitSpotLightComponent(Entity* entity);
	void InitCameraComponent(Entity* entity);
	void InitBoundingBoxComponent(Entity* entity);
	void InitTextComponent(Entity* entity);

private:
	friend class SceneManager;
	friend class Text;
	Renderer();

	ThreadPool* m_pThreadPool = nullptr;

	// Camera
	BaseCamera* m_pScenePrimaryCamera = nullptr;

	unsigned int m_FrameCounter = 0;

	// Device
	ID3D12Device5* m_pDevice5 = nullptr;

	// CommandQueues
	std::map<COMMAND_INTERFACE_TYPE, ID3D12CommandQueue*> m_CommandQueues;

	// RenderTargets
	// Swapchain (inheriting from 'RenderTarget')
	SwapChain* m_pSwapChain = nullptr;
	
	// Bloom (includes rtv, uav and srv)
	Bloom* m_pBloomResources = nullptr;

	// Depthbuffer
	DepthStencil* m_pMainDepthStencil = nullptr;

	// Rootsignature
	RootSignature* m_pRootSignature = nullptr;

	// Picking
	MousePicker* m_pMousePicker = nullptr;
	Entity* m_pPickedEntity = nullptr;

	// Tasks
	std::vector<ComputeTask*> m_ComputeTasks;
	std::vector<CopyTask*>    m_CopyTasks;
	std::vector<RenderTask*>  m_RenderTasks;

	// Since these tasks wont operate on all objects, they will not be set in the same map as the other "rendertasks".
	//WireframeRenderTask* m_pWireFrameTask = nullptr;
	//OutliningRenderTask* m_pOutliningRenderTask = nullptr;	

	Mesh* m_pFullScreenQuad = nullptr;

	// Group of components that's needed for rendering:
	std::map<FLAG_DRAW, std::vector<std::pair<component::ModelComponent*, component::TransformComponent*>>> m_RenderComponents;
	std::vector<component::BoundingBoxComponent*> m_BoundingBoxesToBePicked;
	std::vector<component::TextComponent*> m_TextComponents;

	ViewPool* m_pViewPool = nullptr;
	std::map<LIGHT_TYPE, std::vector<std::tuple<Light*, ConstantBuffer*, ShadowInfo*>>> m_Lights;

	// Current scene to be drawn
	Scene* m_pCurrActiveScene = nullptr;
	CB_PER_SCENE_STRUCT* m_pCbPerSceneData = nullptr;
	ConstantBuffer* m_pCbPerScene = nullptr;

	// update per frame
	CB_PER_FRAME_STRUCT* m_pCbPerFrameData = nullptr;
	ConstantBuffer* m_pCbPerFrame = nullptr;

	// Commandlists holders
	std::vector<ID3D12CommandList*> m_DirectCommandLists[NUM_SWAP_BUFFERS];
	std::vector<ID3D12CommandList*> m_ComputeCommandLists[NUM_SWAP_BUFFERS];
	ID3D12CommandList* m_CopyOnDemandCmdList[NUM_SWAP_BUFFERS];
	
	// DescriptorHeaps
	std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> m_DescriptorHeaps = {};

	// Fences
	HANDLE m_EventHandle = nullptr;
	ID3D12Fence1* m_pFenceFrame = nullptr;
	UINT64 m_FenceFrameValue = 0;

	void setRenderTasksPrimaryCamera();
	bool createDevice();
	void createCommandQueues();
	void createSwapChain(const HWND *hwnd);
	void createMainDSV();
	void createRootSignature();
	void createFullScreenQuad();
	void updateMousePicker();
	void initRenderTasks();
	void setRenderTasksRenderComponents();
	void createDescriptorHeaps();
	void createFences();
	void waitForFrame(unsigned int framesToBeAhead = NUM_SWAP_BUFFERS - 1);

	// WaitForFrame but with the copyqueue only. Is used when executing per scene data on SetScene
	void waitForCopyOnDemand();

	// Manage components
	void removeComponents(Entity* entity);
	void addComponents(Entity* entity);

	// Setup the whole scene
	void prepareScene(Scene* scene);
	// Setup Per-scene data and send to GPU
	void prepareCBPerScene();
	// Submit per-frame data to the copyQueue that updates each frame
	void prepareCBPerFrame();
};

#endif