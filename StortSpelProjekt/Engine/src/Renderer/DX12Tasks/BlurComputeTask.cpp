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

	m_HorizontalThreadGroupsX = static_cast<unsigned int>(ceilf(static_cast<float>(screenWidth) / m_ThreadsPerGroup));
	m_HorizontalThreadGroupsY = screenHeight;

	m_VerticalThreadGroupsX = screenWidth;
	m_VerticalThreadGroupsY = m_HorizontalThreadGroupsX;
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

	// Descriptorheap indices for the textures to blur
	// Horizontal pass
	m_DhIndices.index0 = m_PingPongResources[0]->GetSRV()->GetDescriptorHeapIndex();	// Read
	m_DhIndices.index1 = m_PingPongResources[1]->GetUAV()->GetDescriptorHeapIndex();	// Write
	// Vertical pass
	m_DhIndices.index2 = m_PingPongResources[1]->GetSRV()->GetDescriptorHeapIndex();	// Read
	m_DhIndices.index3 = m_PingPongResources[0]->GetUAV()->GetDescriptorHeapIndex();	// Write

	// Send the indices to gpu
	commandList->SetComputeRoot32BitConstants(RS::CB_INDICES_CONSTANTS, sizeof(DescriptorHeapIndices) / sizeof(UINT), &m_DhIndices, 0);

	unsigned int timesToBlur = 6;
	for (unsigned int i = 0; i < timesToBlur; i++)
	{
		// The resource to read (Resource Barrier)
		TransResourceState(commandList, const_cast<Resource*>(m_PingPongResources[0]->GetSRV()->GetResource()),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		// The resource to write (Resource Barrier)
		TransResourceState(commandList, const_cast<Resource*>(m_PingPongResources[1]->GetUAV()->GetResource()),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// Blur horizontal
		commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());
		commandList->Dispatch(m_HorizontalThreadGroupsX, m_HorizontalThreadGroupsY, 1);

		// The resource to write to (Resource Barrier)
		TransResourceState(commandList, const_cast<Resource*>(m_PingPongResources[0]->GetUAV()->GetResource()),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// The resource to read (Resource Barrier)
		TransResourceState(commandList, const_cast<Resource*>(m_PingPongResources[1]->GetSRV()->GetResource()),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		// Blur vertical
		commandList->SetPipelineState(m_PipelineStates[1]->GetPSO());
		commandList->Dispatch(m_VerticalThreadGroupsX, m_VerticalThreadGroupsY, 1);


		// Resource barrier back to original states
		TransResourceState(commandList, const_cast<Resource*>(m_PingPongResources[0]->GetUAV()->GetResource()),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	commandList->Close();
}
