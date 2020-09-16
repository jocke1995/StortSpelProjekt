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
		std::vector<std::pair<LPCWSTR, LPCTSTR>> csNamePSOName,
		COMMAND_INTERFACE_TYPE interfaceType,
		const PingPongResource* Bloom0_RESOURCE,
		const PingPongResource* Bloom1_RESOURCE,
		unsigned int screenWidth, unsigned int screenHeight
		);
	virtual ~BlurComputeTask();

	void Execute();
private:
	std::array<const PingPongResource*, 2> m_PingPongResources;
	unsigned int m_HorizontalThreadGroupsX;
	unsigned int m_HorizontalThreadGroupsY;
	unsigned int m_VerticalThreadGroupsX;
	unsigned int m_VerticalThreadGroupsY;
	const unsigned int m_ThreadsPerGroup = 256;

	DescriptorHeapIndices m_DhIndices = {};
};

#endif