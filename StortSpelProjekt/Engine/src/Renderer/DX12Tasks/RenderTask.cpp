#include "stdafx.h"
#include "RenderTask.h"

#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../GraphicsState.h"
#include "../Resource.h"
#include "../SwapChain.h"

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
	for (auto pipelineState : m_PipelineStates)
		delete pipelineState;
}

PipelineState* RenderTask::GetPipelineState(unsigned int index)
{
	return m_PipelineStates[index];
}

void RenderTask::AddRenderTarget(std::string name, const RenderTarget* renderTarget)
{
	m_RenderTargets[name] = renderTarget;
}

void RenderTask::SetRenderComponents(std::vector<std::pair<	component::MeshComponent*,
															component::TransformComponent*>>*renderComponents)
{
	m_RenderComponents = *renderComponents;
}

void RenderTask::SetCamera(BaseCamera* camera)
{
	m_pCamera = camera;
}

void RenderTask::SetSwapChain(SwapChain* swapChain)
{
	m_pSwapChain = swapChain;
}



