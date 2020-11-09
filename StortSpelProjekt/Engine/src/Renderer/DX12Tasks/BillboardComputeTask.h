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

	void SetParticleEffects(std::vector<ParticleEffect*>* particleEffects);

	void Execute();
private:
	friend class ParticleRenderTask;

	std::vector<ParticleEffect*> m_ParticleEffects;

	unsigned int m_ThreadGroupsX = 1;
	unsigned int m_ThreadGroupsY = 1;

	const unsigned int m_ThreadsPerGroup = 256;
};

#endif