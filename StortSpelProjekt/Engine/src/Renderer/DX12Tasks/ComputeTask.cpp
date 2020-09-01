#include "stdafx.h"
#include "ComputeTask.h"

#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../ComputeState.h"

ComputeTask::ComputeTask(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR CSName, LPCTSTR psoName)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::COMPUTE_TYPE)
{
	m_pPipelineState = new ComputeState(device, rootSignature, CSName, psoName);

	m_pRootSig = rootSignature->GetRootSig();
}

ComputeTask::~ComputeTask()
{
	delete m_pPipelineState;
}
