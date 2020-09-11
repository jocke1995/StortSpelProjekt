#ifndef BRIGHTBLURTASK_H
#define BRIGHTBLURTASK_H

#include "ComputeTask.h"
class PingPongResource;

class BlurComputeTask : public ComputeTask
{
public:
	BlurComputeTask(
		ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR CSName,
		LPCTSTR psoName,
		COMMAND_INTERFACE_TYPE interfaceType,
		const PingPongResource* Bloom0_RESOURCE,
		const PingPongResource* Bloom1_RESOURCE
		);
	virtual ~BlurComputeTask();

	void Execute();
private:
	std::array<const PingPongResource*, 2> m_PingPongResources;
};

#endif