#include "stdafx.h"
#include "ComputeTask.h"

#include "../RootSignature.h"
#include "../ComputeState.h"

ComputeTask::ComputeTask(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR CSName, LPCTSTR psoName, COMMAND_INTERFACE_TYPE interfaceType)
	:DX12Task(device, interfaceType)
{
	m_pPipelineState = new ComputeState(device, rootSignature, CSName, psoName);

	m_pRootSig = rootSignature->GetRootSig();
}

ComputeTask::~ComputeTask()
{
	delete m_pPipelineState;
}
