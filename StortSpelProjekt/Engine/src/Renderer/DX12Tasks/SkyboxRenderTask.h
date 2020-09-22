#ifndef SKYBOXTASK_H
#define SKYBOXTASK_H

#include "RenderTask.h"

namespace component
{
	class SkyboxComponent;
}

class ID3D12SamplerState;

class SkyboxRenderTask : public RenderTask
{
public:
	SkyboxRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~SkyboxRenderTask();

	void SetSkybox(component::SkyboxComponent* skybox);

	void Execute();

private:
	component::SkyboxComponent* m_Skybox = nullptr;

	ID3D12SamplerState* cubemapSampler;
};

#endif