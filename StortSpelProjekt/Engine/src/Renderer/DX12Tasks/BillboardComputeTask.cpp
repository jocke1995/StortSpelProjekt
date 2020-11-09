#include "stdafx.h"
#include "BillboardComputeTask.h"

#include "../CommandInterface.h"
#include"../RootSignature.h"
#include"../PipelineState.h"

#include "../GPUMemory/ShaderResourceView.h"
#include "../GPUMemory/UnorderedAccessView.h"
#include "../GPUMemory/Resource.h"
#include "../DescriptorHeap.h"

#include "../Renderer/Mesh.h"

#include "../Particles/ParticleEffect.h"

BillboardComputeTask::BillboardComputeTask(ID3D12Device5* device, RootSignature* rootSignature, std::vector<std::pair<std::wstring, std::wstring>> csNamePSOName, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD)
	: ComputeTask(device, rootSignature, csNamePSOName, FLAG_THREAD, interfaceType)
{
}

BillboardComputeTask::~BillboardComputeTask()
{
}

void BillboardComputeTask::SetParticleEffects(std::vector<ParticleEffect*>* particleEffects)
{
	m_ParticleEffects = *particleEffects;
}

void BillboardComputeTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);

	commandAllocator->Reset();
	commandList->Reset(commandAllocator, NULL);

	commandList->SetComputeRootSignature(m_pRootSig);

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	

	
	
	for (unsigned int i = 0; i < m_ParticleEffects.size(); i++)
	{
		//// The resource to read (Resource Barrier)
		//TransResourceState(commandList, const_cast<Resource*>(m_pSRV[i]->GetResource()),
		//	D3D12_RESOURCE_STATE_RENDER_TARGET,
		//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		//
		//// The resource to write (Resource Barrier)
		//TransResourceState(commandList, const_cast<Resource*>(m_pUAV[i]->GetResource()),
		//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		//	D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// Descriptorheap indices for what mesh is used
		
		commandList->SetComputeRootDescriptorTable(RS::dtSRV0, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(m_ParticleEffects.at(i)->m_pSRV->GetDescriptorHeapIndex()));
		commandList->SetComputeRootDescriptorTable(RS::dtUAV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(m_ParticleEffects.at(i)->m_pUAV->GetDescriptorHeapIndex()));

		m_ThreadGroupsX = 5;
		commandList->Dispatch(m_ThreadGroupsX, m_ThreadGroupsY, 1);
	}

	commandList->Close();
}
