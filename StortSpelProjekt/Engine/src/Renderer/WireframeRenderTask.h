#ifndef WIREFRAMERENDERTASK_H
#define WIREFRAMERENDERTASK_H

#include "RenderTask.h"
#include "GraphicsState.h"
#include "SwapChain.h"

#include "../Headers/stdafx.h"

#include "../ECS/Components/BoundingBoxComponent.h"

class WireframeRenderTask : public RenderTask
{
public:
	WireframeRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~WireframeRenderTask();

	void AddObjectToDraw(component::BoundingBoxComponent* bbc);

	void Clear();
	void ClearSpecific(component::BoundingBoxComponent* bbc);

	void Execute();

private:
	std::vector<component::BoundingBoxComponent*> objectsToDraw;
};

#endif