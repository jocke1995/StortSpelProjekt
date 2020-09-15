#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "Core.h"
#include <map>
#include "DX12Task.h"

// Renderer
class RootSignature;
class Resource;

class BaseCamera;
class RenderTarget;
class SwapChain;
class PipelineState;

// Components
#include "../../ECS/Components/ModelComponent.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/TextComponent.h"
#include "../../ECS/Components/Lights/DirectionalLightComponent.h"
#include "../../ECS/Components/Lights/PointLightComponent.h"
#include "../../ECS/Components/Lights/SpotLightComponent.h"

#include "../Text.h"

// DX12 Forward Declarations
struct ID3D12RootSignature;
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;

class RenderTask : public DX12Task
{
public:
	RenderTask(ID3D12Device5* device, 
		RootSignature* rootSignature, 
		LPCWSTR VSName, LPCWSTR PSName, 
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*> *gpsds,
		LPCTSTR psoName);
	
	virtual ~RenderTask();

	PipelineState* GetPipelineState(unsigned int index);

	
	void AddRenderTarget(std::string, const RenderTarget* renderTarget);
	
	void SetRenderComponents(
		std::vector<std::pair<	component::ModelComponent*,
								component::TransformComponent*>>* renderComponents);

	void SetCamera(BaseCamera* camera);
	void SetSwapChain(SwapChain* swapChain);
	
protected:
	std::vector<std::pair<component::ModelComponent*, component::TransformComponent*>> m_RenderComponents;
	std::map<std::string, const RenderTarget*> m_RenderTargets;
	
	BaseCamera* m_pCamera = nullptr;
	SwapChain* m_pSwapChain = nullptr;
	ID3D12RootSignature* m_pRootSig = nullptr;
	std::vector<PipelineState*> m_PipelineStates;
};
#endif
