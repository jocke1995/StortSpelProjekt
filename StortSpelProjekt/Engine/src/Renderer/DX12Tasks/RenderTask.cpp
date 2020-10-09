#include "stdafx.h"
#include "RenderTask.h"

#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../GraphicsState.h"
#include "../GPUMemory/Resource.h"
#include "../SwapChain.h"
#include "../GPUMemory/DepthStencil.h"

RenderTask::RenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	const std::wstring& VSName, const std::wstring& PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	const std::wstring& psoName,
	unsigned int FLAG_THREAD)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::DIRECT_TYPE, FLAG_THREAD)
{
	if (gpsds != nullptr)
	{
		for (auto gpsd : *gpsds)
		{
			m_PipelineStates.push_back(new GraphicsState(device, rootSignature, VSName, PSName, gpsd, psoName));
		}
	}
	
	m_pRootSig = rootSignature->GetRootSig();
}

RenderTask::~RenderTask()
{
	for (auto pipelineState : m_PipelineStates)
		delete pipelineState;
}

PipelineState* RenderTask::GetPipelineState(unsigned int index)
{
	return m_PipelineStates[index];
}

void RenderTask::AddRenderTargetView(std::string name, const RenderTargetView* renderTarget)
{
	m_RenderTargets[name] = renderTarget;
}

void RenderTask::SetRenderComponents(std::vector<std::pair<	component::ModelComponent*,
															component::TransformComponent*>>*renderComponents)
{
	m_RenderComponents = *renderComponents;
}

void RenderTask::SetMainDepthStencil(DepthStencil* depthStencil)
{
	m_pDepthStencil = depthStencil;
}

void RenderTask::SetCamera(BaseCamera* camera)
{
	m_pCamera = camera;
}

void RenderTask::SetSwapChain(SwapChain* swapChain)
{
	m_pSwapChain = swapChain;
}



