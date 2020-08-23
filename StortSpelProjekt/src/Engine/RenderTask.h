#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "DX12Task.h"

#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "GraphicsState.h"
#include "SwapChain.h"

#include "../ECS/Components/MeshComponent.h"
#include "../ECS/Components/TransformComponent.h"

#include "../ECS/Components/Lights/DirectionalLightComponent.h"
#include "../ECS/Components/Lights/PointLightComponent.h"
#include "../ECS/Components/Lights/SpotLightComponent.h"

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

	void AddResource(std::string id, Resource* resource);
	void AddRenderTarget(std::string, RenderTarget* renderTarget);
	void SetDescriptorHeaps(std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> dhs);

	void SetRenderComponents(
		std::vector<std::pair<	component::MeshComponent*,
								component::TransformComponent*>>* renderComponents);

	void SetCamera(BaseCamera* camera);
	
protected:
	std::map<std::string, Resource*> resources;
	std::map<std::string, RenderTarget*> renderTargets;
	std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> descriptorHeaps;

	std::vector<std::pair<component::MeshComponent*, component::TransformComponent*>> renderComponents;
	
	BaseCamera* camera = nullptr;
	ID3D12RootSignature* rootSig = nullptr;
	std::vector<PipelineState*> pipelineStates;
};
#endif
