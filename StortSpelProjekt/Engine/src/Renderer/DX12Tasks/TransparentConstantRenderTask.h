#ifndef BLENDRENDERTASK_H
#define BLENDRENDERTASK_H

#include "RenderTask.h"

class RootSignature;

class TransparentConstantRenderTask : public RenderTask
{
public:
	TransparentConstantRenderTask(ID3D12Device5* device, 
		RootSignature* rootSignature, 
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	~TransparentConstantRenderTask();

	void Execute();

private:

};

#endif