#ifndef BILLBOARDTASK_H
#define BILLBOARDTASK_H

#include "ComputeTask.h"
class ShaderResourceView;
class UnorderedAccessView;

class ParticleEffect;

class BillboardComputeTask : public ComputeTask
{
public:
	BillboardComputeTask(
		ID3D12Device5* device,
		RootSignature* rootSignature,
		std::vector<std::pair<std::wstring, std::wstring>> csNamePSOName,
		COMMAND_INTERFACE_TYPE interfaceType,
		unsigned int FLAG_THREAD
	);
	virtual ~BillboardComputeTask();

	void SetParticleEffects(std::vector<ParticleEffect*> particleEffects);

	void Execute();
private:
	std::vector<ShaderResourceView*> m_pSRV;
	std::vector<UnorderedAccessView*> m_pUAV;

	unsigned int m_ThreadGroupsX = 0;
	unsigned int m_ThreadGroupsY = 0;

	const unsigned int m_ThreadsPerGroup = 256;

	DescriptorHeapIndices m_DhIndices = {};
};

#endif