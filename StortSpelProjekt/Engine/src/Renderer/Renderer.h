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
class Texture;
class Model;
class Resource;
class Text;

// Views

// GPU Resources
class ConstantBuffer;
class ShaderResource;
class UnorderedAccess;
class DepthStencil;
class Resource;

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
class Material;

// Copy
class CopyTask;

// Compute
class ComputeTask;

// DX12 Forward Declarations
struct ID3D12CommandQueue;
struct ID3D12CommandList;
struct ID3D12Fence1;
struct ID3D12Device5;
struct IDXGIAdapter4;

// ECS
class Entity;
namespace component
{
	class ModelComponent;
	class AnimationComponent;
	class TransformComponent;
	class CameraComponent;
	class BoundingBoxComponent;
	class GUI2DComponent;
	class SkyboxComponent;
	class DirectionalLightComponent;
	class PointLightComponent;
	class SpotLightComponent;
	class ParticleEmitterComponent;
	class ProgressBarComponent;
}

// Events
struct WindowChange;
struct WindowSettingChange;

class Renderer
{
public:
	static Renderer& GetInstance();
	virtual ~Renderer();

	// PickedEntity
	Entity* const GetPickedEntity() const;
	// Scene
	Scene* const GetActiveScene() const;

	Window* const GetWindow() const;

	// Call once
	void InitD3D12(Window* window, HINSTANCE hInstance, ThreadPool* threadPool);

	// Call on logic update *This should be moved to a more relevant logic class
	void Update(double dt);
	// Call each frame
	void RenderUpdate(double dt); //Please rename if logic update is removed
	void SortObjects();
	void Execute();

	// Render inits, these functions are called by respective components through SetScene to prepare for drawing
	void InitSkyboxComponent(component::SkyboxComponent* component);
	void InitModelComponent(component::ModelComponent* component);
	void InitAnimationComponent(component::AnimationComponent* component);
	void InitDirectionalLightComponent(component::DirectionalLightComponent* component);
	void InitPointLightComponent(component::PointLightComponent* component);
	void InitSpotLightComponent(component::SpotLightComponent* component);
	void InitCameraComponent(component::CameraComponent* component);
	void InitBoundingBoxComponent(component::BoundingBoxComponent* component);
	void InitGUI2DComponent(component::GUI2DComponent* component);
	void InitParticleEmitterComponent(component::ParticleEmitterComponent* component);
	void InitProgressBarComponent(component::ProgressBarComponent* component);

	void UnInitSkyboxComponent(component::SkyboxComponent* component);
	void UnInitModelComponent(component::ModelComponent* component);
	void UnInitAnimationComponent(component::AnimationComponent* component);
	void UnInitDirectionalLightComponent(component::DirectionalLightComponent* component);
	void UnInitPointLightComponent(component::PointLightComponent* component);
	void UnInitSpotLightComponent(component::SpotLightComponent* component);
	void UnInitCameraComponent(component::CameraComponent* component);
	void UnInitBoundingBoxComponent(component::BoundingBoxComponent* component);
	void UnInitGUI2DComponent(component::GUI2DComponent* component);
	void UnInitParticleEmitterComponent(component::ParticleEmitterComponent* component);
	void UnInitProgressBarComponent(component::ProgressBarComponent* component);

	void SetBrightness(float value);

	void OnResetScene();

private:
	friend class Engine;
	friend class component::SkyboxComponent;
	friend class component::GUI2DComponent;
	friend class component::ParticleEmitterComponent;
	friend class SceneManager;
	friend class TextManager;
	friend class QuadManager;
	friend class ParticleSystem;
	friend class ParticleEffect;
	friend class ImGuiHandler;
	friend class ShadowInfo;
	Renderer();

	// For control of safe release of DirectX resources
	void deleteRenderer();

	// SubmitToCodt functions
	void submitToCodt(std::tuple<Resource*, Resource*, const void*>* Upload_Default_Data);
	void submitModelToGPU(Model* model);
	void submitMeshToCodt(Mesh* mesh);
	void submitTextureToCodt(Texture* texture);

	//SubmitToCpft functions
	void submitToCpft(std::tuple<Resource*, Resource*, const void*>* Upload_Default_Data);
	void clearSpecificCpft(Resource* upload);

	DescriptorHeap* getCBVSRVUAVdHeap() const;

	ThreadPool* m_pThreadPool = nullptr;

	// Camera
	BaseCamera* m_pScenePrimaryCamera = nullptr;

	unsigned int m_FrameCounter = 0;

	// Window
	Window* m_pWindow;

	// Device
	ID3D12Device5* m_pDevice5 = nullptr;

	// Adapters used for getting VRAM and RAM
	IDXGIAdapter4* m_pAdapter4 = nullptr;
	HANDLE m_ProcessHandle = nullptr;

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
	Mesh* m_pQuadMesh = nullptr;

	// Group of components that's needed for rendering:
	std::map<FLAG_DRAW, std::vector<std::pair<component::ModelComponent*, component::TransformComponent*>>> m_RenderComponents;
	std::vector<component::BoundingBoxComponent*> m_BoundingBoxesToBePicked;
	std::vector<component::GUI2DComponent*> m_TextComponents;
	std::vector<component::GUI2DComponent*> m_QuadComponents;
	std::vector<component::ProgressBarComponent*> m_ProgressBarComponents;

	component::SkyboxComponent* m_pSkyboxComponent = nullptr;

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
	
	// DescriptorHeaps
	std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> m_DescriptorHeaps = {};

	// Fences
	HANDLE m_EventHandle = nullptr;
	ID3D12Fence1* m_pFenceFrame = nullptr;
	UINT64 m_FenceFrameValue = 0;

	// Textures to remove from RAM after they've been uploaded to the GPU.
	std::vector<Texture*> m_TexturesToRemove;

	void setRenderTasksPrimaryCamera();
	bool createDevice();
	void createCommandQueues();
	void createSwapChain();
	void createMainDSV();
	void createRootSignature();
	void createFullScreenQuad();
	void updateMousePicker();
	void initRenderTasks();
	void setRenderTasksRenderComponents();
	void createDescriptorHeaps();
	void createFences();
	void waitForFrame(unsigned int framesToBeAhead = NUM_SWAP_BUFFERS - 1);
	void waitForGPU();

	// bad design but its ok since we have focus on getting the game to work
	void setRenderTasksGUI2DComponents();
	void setProgressBarComponents();

	// WaitForFrame but with the copyqueue only. Is used when executing per scene data on SetScene
	//void waitForCopyOnDemand();
	//void executeCopyOnDemand();

	// Setup the whole scene
	void prepareScene(Scene* activeScene);

	// Submit cbPerSceneData to the copyQueue that updates once
	void submitUploadPerSceneData();
	// Submit cbPerFrameData to the copyQueue that updates each frame
	void submitUploadPerFrameData();

	void toggleFullscreen(WindowChange* evnt);

	SwapChain* getSwapChain() const;

	void submitTextToGPU(Text* text, TextManager* tm);
};

#endif