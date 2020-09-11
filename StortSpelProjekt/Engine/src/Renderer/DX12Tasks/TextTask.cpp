#include "stdafx.h"
#include "TextTask.h"

TextTask::TextTask(ID3D12Device5* device, 
	RootSignature* rootSignature, 
	LPCWSTR VSName, LPCWSTR PSName, 
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
	LPCTSTR psoName) :RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
}

TextTask::~TextTask()
{
}

void TextTask::Execute()
{
}
