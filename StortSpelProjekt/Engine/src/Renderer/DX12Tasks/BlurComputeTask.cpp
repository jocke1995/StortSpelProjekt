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
	LPCWSTR CSName, LPCTSTR psoName,
	COMMAND_INTERFACE_TYPE interfaceType,
	const PingPongResource* Bloom0_RESOURCE,
	const PingPongResource* Bloom1_RESOURCE)
	:ComputeTask(device, rootSignature, CSName, psoName, interfaceType)
{
	m_PingPongResources[0] = Bloom0_RESOURCE;
	m_PingPongResources[1] = Bloom1_RESOURCE;
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

	commandList->SetPipelineState(m_pPipelineState->GetPSO());
	

	// The resource to read (Resource Barrier)
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	m_PingPongResources[0]->GetSRV()->GetResource()->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
	
	
	// The resource to write (Resource Barrier)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_PingPongResources[1]->GetSRV()->GetResource()->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
	
	unsigned int numGroupsX = static_cast<unsigned int>(ceilf(800 / 256.0f));
	unsigned int numGroupsY = 600;
	commandList->Dispatch(numGroupsX, numGroupsY, 1);
	
	
	// The resource to read (Resource Barrier)
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	m_PingPongResources[0]->GetSRV()->GetResource()->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET));

	// The resource to write (Resource Barrier)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_PingPongResources[1]->GetSRV()->GetResource()->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

	commandList->Close();
}
