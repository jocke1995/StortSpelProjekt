#ifndef OUTLININGRENDERTASK_H
#define OUTLININGRENDERTASK_H

#include "RenderTask.h"

class GraphicsState;
class SwapChain;

class OutliningRenderTask : public RenderTask
{
public:
	OutliningRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~OutliningRenderTask();

	void Execute();

	void SetObjectToOutline(std::pair<component::ModelComponent*, component::TransformComponent*>* objectToOutline);
	void Clear();
private:
	std::pair<component::ModelComponent*, component::TransformComponent*> m_ObjectToOutline;
};

#endif