#include "stdafx.h"
#include "BlurComputeTask.h"

#include "../CommandInterface.h"
#include"../RootSignature.h"
#include "../ComputeState.h"

#include "../PingPongResource.h"
#include "../ShaderResourceView.h"
#include "../UnorderedAccessView.h"
#include "../Resource.h"
#include "../DescriptorHeap.h"

BlurComputeTask::BlurComputeTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	std::vector<std::pair< LPCWSTR, LPCTSTR>> csNamePSOName,
	COMMAND_INTERFACE_TYPE interfaceType,
	const PingPongResource* Bloom0_RESOURCE,
	const PingPongResource* Bloom1_RESOURCE,
	unsigned int screenWidth, unsigned int screenHeight)
	:ComputeTask(device, rootSignature, csNamePSOName, interfaceType)
{
	m_PingPongResources[0] = Bloom0_RESOURCE;
	m_PingPongResources[1] = Bloom1_RESOURCE;

	m_threadGroupsX = static_cast<unsigned int>(ceilf(static_cast<float>(screenWidth) / m_threadsPerGroup));
	m_threadGroupsY = screenHeight;
}

BlurComputeTask::~BlurComputeTask()
{

}

void BlurComputeTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	
	commandAllocator->Reset();
	commandList->Reset(commandAllocator, NULL);
	
	commandList->SetComputeRootSignature(m_pRootSig);
	
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetComputeRootDescriptorTable(RS::dtUAV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));
	commandList->SetComputeRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());
	

	// The resource to read (Resource Barrier)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_PingPongResources[0]->GetSRV()->GetResource()->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
	
	
	// The resource to write (Resource Barrier)
	TransResourceState(commandList, const_cast<Resource*>(m_PingPongResources[1]->GetSRV()->GetResource()),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	
	commandList->Dispatch(m_threadGroupsX, m_threadGroupsY, 1);
	
	
	// The resource to read (Resource Barrier)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_PingPongResources[0]->GetSRV()->GetResource()->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	// The resource to write (Resource Barrier)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_PingPongResources[1]->GetSRV()->GetResource()->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

	commandList->Close();
}
