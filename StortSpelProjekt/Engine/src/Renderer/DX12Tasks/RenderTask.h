#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "Core.h"
#include <map>
#include "DX12Task.h"

// Renderer
class RootSignature;
class Resource;
class DescriptorHeap;
enum class DESCRIPTOR_HEAP_TYPE;
class BaseCamera;
class RenderTarget;
class PipelineState;

// Components
#include "../../ECS/Components/MeshComponent.h"
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

	void AddResource(std::string id, Resource* resource);
	void AddRenderTarget(std::string, RenderTarget* renderTarget);
	void SetDescriptorHeaps(std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> dhs);

	void SetRenderComponents(
		std::vector<std::pair<	component::MeshComponent*,
								component::TransformComponent*>>* renderComponents);
	void SetTextComponents(std::vector<component::TextComponent*>* textComponents);

	void SetCamera(BaseCamera* camera);
	
protected:
	std::map<std::string, Resource*> m_Resources;
	std::map<std::string, RenderTarget*> m_RenderTargets;
	std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> m_DescriptorHeaps;

	std::vector<std::pair<component::MeshComponent*, component::TransformComponent*>> m_RenderComponents;
	std::vector<component::TextComponent*> m_TextComponents;
	
	BaseCamera* m_pCamera = nullptr;
	ID3D12RootSignature* m_pRootSig = nullptr;
	std::vector<PipelineState*> m_PipelineStates;
};
#endif
