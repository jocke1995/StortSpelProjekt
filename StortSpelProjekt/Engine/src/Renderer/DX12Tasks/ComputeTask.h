#ifndef COMPUTETASK_H
#define COMPUTETASK_H

#include "DX12Task.h"
#include "../CommandInterface.h"

class RootSignature;
class PipelineState;

class ComputeTask : public DX12Task
{
public:
	ComputeTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		std::vector<std::pair< std::wstring, std::wstring>> csNamePSOName,
		unsigned int FLAG_THREAD,
		COMMAND_INTERFACE_TYPE interfaceType = COMMAND_INTERFACE_TYPE::COMPUTE_TYPE);
	virtual ~ComputeTask();

protected:
	ID3D12RootSignature* m_pRootSig = nullptr;

	std::vector<PipelineState*> m_PipelineStates;
};
#endif
