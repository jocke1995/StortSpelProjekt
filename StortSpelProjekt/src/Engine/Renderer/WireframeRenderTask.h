#ifndef WIREFRAMERENDERTASK_H
#define WIREFRAMERENDERTASK_H

#include "RenderTask.h"
#include "GraphicsState.h"
#include "SwapChain.h"

class WireframeRenderTask : public RenderTask
{
public:
	WireframeRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~WireframeRenderTask();

	void AddObjectToDraw(std::pair<Mesh*, Transform*>* pair);
	void Clear();

	void Execute();

private:
	std::vector<std::pair<Mesh*, Transform*>> objectsToDraw;
};

#endif