#ifndef BRIGHTBLURTASK_H
#define BRIGHTBLURTASK_H

#include "ComputeTask.h"
class BrightBlurTask : public ComputeTask
{
public:
	BrightBlurTask(
		ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR CSName,
		LPCTSTR psoName);
	virtual ~BrightBlurTask();

	void Execute();
private:
};

#endif