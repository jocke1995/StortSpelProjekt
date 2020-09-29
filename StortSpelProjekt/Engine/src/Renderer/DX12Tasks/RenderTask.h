#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "Core.h"
#include <map>
#include "DX12Task.h"

// Renderer
class RootSignature;
class Resource;

class DepthStencil;
class BaseCamera;
class RenderTargetView;
class SwapChain;
class PipelineState;

// Components
#include "../../ECS/Components/ModelComponent.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/Lights/DirectionalLightComponent.h"
#include "../../ECS/Components/Lights/PointLightComponent.h"
#include "../../ECS/Components/Lights/SpotLightComponent.h"

// DX12 Forward Declarations
struct ID3D12RootSignature;
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;

class RenderTask : public DX12Task
{
public:
	RenderTask(ID3D12Device5* device, 
		RootSignature* rootSignature, 
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> *gpsds,
		const std::wstring& psoName);
	
	virtual ~RenderTask();

	PipelineState* GetPipelineState(unsigned int index);

	
	void AddRenderTarget(std::string, const RenderTargetView* renderTarget);
	
	void SetRenderComponents(
		std::vector<std::pair<	component::ModelComponent*,
								component::TransformComponent*>>* renderComponents);
	void SetMainDepthStencil(DepthStencil* depthStencil);

	void SetCamera(BaseCamera* camera);
	void SetSwapChain(SwapChain* swapChain);
	
protected:
	std::vector<std::pair<component::ModelComponent*, component::TransformComponent*>> m_RenderComponents;
	std::map<std::string, const RenderTargetView*> m_RenderTargets;
	
	DepthStencil* m_pDepthStencil = nullptr;
	BaseCamera* m_pCamera = nullptr;
	SwapChain* m_pSwapChain = nullptr;
	ID3D12RootSignature* m_pRootSig = nullptr;
	std::vector<PipelineState*> m_PipelineStates;
};
#endif
