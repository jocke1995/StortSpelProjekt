#ifndef OUTLININGRENDERTASK_H
#define OUTLININGRENDERTASK_H

#include "RenderTask.h"
#include "GraphicsState.h"
#include "SwapChain.h"

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

	void SetObjectToOutline(std::pair<component::MeshComponent*, component::TransformComponent*>* objectToOutline);
	void Clear();
private:
	std::pair<component::MeshComponent*, component::TransformComponent*> objectToOutline;
};

#endif