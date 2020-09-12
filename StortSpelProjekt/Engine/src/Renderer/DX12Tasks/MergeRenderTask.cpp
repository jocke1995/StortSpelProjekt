#include "stdafx.h"
#include "MergeRenderTask.h"

MergeRenderTask::MergeRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
}

MergeRenderTask::~MergeRenderTask()
{
}

void MergeRenderTask::Execute()
{
}
