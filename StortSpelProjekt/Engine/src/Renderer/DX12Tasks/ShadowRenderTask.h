#ifndef SHADOWRENDERTASK_H
#define SHADOWRENDERTASK_H

#include "RenderTask.h"

class ShadowInfo;

class ShadowRenderTask : public RenderTask
{
public:
	ShadowRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~ShadowRenderTask();
	void AddShadowCastingLight(std::pair<Light*, ShadowInfo*> light);

	void Execute();
private:
	std::vector<std::pair<Light*, ShadowInfo*>> m_lights;
};

#endif