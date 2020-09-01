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
		this->pipelineStates.push_back(new GraphicsState(device, rootSignature, VSName, PSName, gpsd, psoName));

	this->rootSig = rootSignature->GetRootSig();
}

RenderTask::~RenderTask()
{
	for (auto pair : this->resources)
	{
		ID3D12Resource1* res = pair.second->GetID3D12Resource1();
		SAFE_RELEASE(&res);
	}

	for (auto pipelineState : this->pipelineStates)
		delete pipelineState;
}

PipelineState* RenderTask::GetPipelineState(unsigned int index)
{
	return this->pipelineStates[index];
}

void RenderTask::AddResource(std::string id, Resource* resource)
{
	if (this->resources[id] == nullptr)
	{
		this->resources[id] = resource;
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
	this->renderTargets[name] = renderTarget;
}

void RenderTask::SetDescriptorHeaps(std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> dhs)
{
	this->descriptorHeaps = dhs;
}

void RenderTask::SetRenderComponents(std::vector<std::pair<	component::MeshComponent*,
															component::TransformComponent*>>*renderComponents)
{
	this->renderComponents = *renderComponents;
}

void RenderTask::SetCamera(BaseCamera* camera)
{
	this->camera = camera;
}



