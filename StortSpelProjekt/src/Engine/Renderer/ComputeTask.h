#ifndef COMPUTETASK_H
#define COMPUTETASK_H

#include "DX12Task.h"
#include "ComputeState.h"

class ComputeTask : public DX12Task
{
public:
	ComputeTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR CSName,
		LPCTSTR psoName);
	virtual ~ComputeTask();

protected:
	ID3D12RootSignature* rootSig = nullptr;

	PipelineState* pipelineState = nullptr;
};
#endif
