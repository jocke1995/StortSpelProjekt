#include "stdafx.h"
#include "RenderTask.h"

#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../GraphicsState.h"
#include "../Resource.h"

RenderTask::RenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::DIRECT_TYPE)
{
	for (auto gpsd : *gpsds)
		m_PipelineStates.push_back(new GraphicsState(device, rootSignature, VSName, PSName, gpsd, psoName));

	m_pRootSig = rootSignature->GetRootSig();
}

RenderTask::~RenderTask()
{
	for (auto pair : m_Resources)
	{
		ID3D12Resource1* res = pair.second->GetID3D12Resource1();
		SAFE_RELEASE(&res);
	}

	for (auto pipelineState : m_PipelineStates)
		delete pipelineState;
}

PipelineState* RenderTask::GetPipelineState(unsigned int index)
{
	return m_PipelineStates[index];
}

void RenderTask::AddResource(std::string id, Resource* resource)
{
	if (m_Resources[id] == nullptr)
	{
		m_Resources[id] = resource;
		resource->GetID3D12Resource1()->AddRef();
		return;
	}
	else
	{
		Log::PrintSeverity(
			Log::Severity::CRITICAL,
			"Trying to add Resource with name: \'%s\' that already exists.\n", id);
	}
}

void RenderTask::AddRenderTarget(std::string name, RenderTarget* renderTarget)
{
	m_RenderTargets[name] = renderTarget;
}

void RenderTask::SetDescriptorHeaps(std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> dhs)
{
	m_DescriptorHeaps = dhs;
}

void RenderTask::SetRenderComponents(std::vector<std::pair<	component::ModelComponent*,
															component::TransformComponent*>>*renderComponents)
{
	m_RenderComponents = *renderComponents;
}

void RenderTask::SetCamera(BaseCamera* camera)
{
	m_pCamera = camera;
}



