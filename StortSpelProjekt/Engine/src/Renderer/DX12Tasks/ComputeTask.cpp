#include "stdafx.h"
#include "ComputeTask.h"

#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../ComputeState.h"

ComputeTask::ComputeTask(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR CSName, LPCTSTR psoName)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::COMPUTE_TYPE)
{
	this->pipelineState = new ComputeState(device, rootSignature, CSName, psoName);

	this->rootSig = rootSignature->GetRootSig();
}

ComputeTask::~ComputeTask()
{
	delete this->pipelineState;
}
