#ifndef BRIGHTBLURTASK_H
#define BRIGHTBLURTASK_H

#include "ComputeTask.h"
class BlurComputeTask : public ComputeTask
{
public:
	BlurComputeTask(
		ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR CSName,
		LPCTSTR psoName,
		COMMAND_INTERFACE_TYPE interfaceType);
	virtual ~BlurComputeTask();

	void Execute();
private:
};

#endif