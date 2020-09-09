#include "stdafx.h"
#include "BlurComputeTask.h"

#include "../CommandInterface.h"
#include"../RootSignature.h"
#include "../ComputeState.h"

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
	
	// Resource with color from the copyQueue -> this ComputeQueue -> DirectQueue
	//commandList->SetComputeRootUnorderedAccessView(RS::ColorUAV,
	//	this->resources[0]->GetGPUVirtualAdress());
	
	commandList->Dispatch(1, 1, 1);
	
	
	commandList->Close();
}
