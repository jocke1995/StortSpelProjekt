#include "stdafx.h"
#include "ComputeTask.h"

#include "../RootSignature.h"
#include "../ComputeState.h"

ComputeTask::ComputeTask(ID3D12Device5* device, RootSignature* rootSignature, std::vector<std::pair<LPCWSTR, LPCTSTR>> csNamePSOName, COMMAND_INTERFACE_TYPE interfaceType)
	:DX12Task(device, interfaceType)
{
	for (auto& pair : csNamePSOName)
	{
		m_PipelineStates.push_back(new ComputeState(device, rootSignature, pair.first, pair.second));
	}

	m_pRootSig = rootSignature->GetRootSig();
}

ComputeTask::~ComputeTask()
{
	for (auto cso : m_PipelineStates)
	{
		delete cso;
	}
	
}
