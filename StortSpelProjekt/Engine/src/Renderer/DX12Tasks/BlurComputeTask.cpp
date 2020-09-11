#include "stdafx.h"
#include "BlurComputeTask.h"

#include "../CommandInterface.h"
#include"../RootSignature.h"
#include "../ComputeState.h"
#include "../Resource.h"

BlurComputeTask::BlurComputeTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR CSName, LPCTSTR psoName,
	COMMAND_INTERFACE_TYPE interfaceType)
	:ComputeTask(device, rootSignature, CSName, psoName, interfaceType)
{

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
	
	commandList->SetPipelineState(m_pPipelineState->GetPSO());
	

	const Resource* readResource = m_Resources["brightResource"];
	const Resource* writeResource = m_Resources["writeResource"];

	// The resource to read (Resource Barrier)
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	readResource->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
	//
	//commandList->SetComputeRootShaderResourceView(RS::SRV1, readResource->GetGPUVirtualAdress());
	//
	////// The resource to write (Resource Barrier)
	////commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	////	writeResource->GetID3D12Resource1(),
	////	D3D12_RESOURCE_STATE_GENERIC_READ,
	////	D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
	////commandList->SetComputeRootUnorderedAccessView(RS::UAV1, writeResource->GetGPUVirtualAdress());
	//
	//unsigned int numGroupsX = static_cast<unsigned int>(ceilf(800 / 256.0f));
	//unsigned int numGroupsY = 600;
	////commandList->Dispatch(numGroupsX, numGroupsY, 1);
	//
	//
	//// The resource to read (Resource Barrier)
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	readResource->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET));

	//// The resource to write (Resource Barrier)
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	writeResource->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	//	D3D12_RESOURCE_STATE_GENERIC_READ));

	commandList->Close();
}
