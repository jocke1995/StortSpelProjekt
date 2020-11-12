#ifndef PARTICLERENDERTASK_H
#define PARTICLERENDERTASK_H

#include "RenderTask.h"

class RootSignature;

class ConstantBufferView;
class Resource;

class ParticleRenderTask : public RenderTask
{
public:
	ParticleRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	~ParticleRenderTask();

	void Execute();

private:
	Mesh* m_pParticleMesh = nullptr;
};

#endif