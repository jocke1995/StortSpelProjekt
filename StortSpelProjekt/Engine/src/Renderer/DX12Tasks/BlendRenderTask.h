#ifndef BLENDRENDERTASK_H
#define BLENDRENDERTASK_H

#include "RenderTask.h"

class RootSignature;

class BlendRenderTask : public RenderTask
{
public:
	BlendRenderTask(ID3D12Device5* device, 
		RootSignature* rootSignature, 
		LPCWSTR VSName, LPCWSTR PSName, 
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~BlendRenderTask();

	void Execute();

private:

};

#endif