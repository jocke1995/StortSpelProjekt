#ifndef SHADOWRENDERTASK_H
#define SHADOWRENDERTASK_H

#include "RenderTask.h"

class ShadowInfo;

class ShadowRenderTask : public RenderTask
{
public:
	ShadowRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName);
	~ShadowRenderTask();
	void AddShadowCastingLight(std::pair<Light*, ShadowInfo*> light);

	void ClearSpecificLight(Light* light);

	void Clear();

	void Execute();
private:
	std::vector<std::pair<Light*, ShadowInfo*>> m_lights;
};

#endif