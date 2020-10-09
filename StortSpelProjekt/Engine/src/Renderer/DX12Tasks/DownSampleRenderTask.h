#ifndef DOWNSAMPLERENDERTASK_H
#define DOWNSAMPLERENDERTASK_H

#include "RenderTask.h"
class Mesh;

class ShaderResourceView;
class UnorderedAccessView;

class DownSampleRenderTask : public RenderTask
{
public:
	DownSampleRenderTask(
		ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		const ShaderResourceView* sourceSRV,
		const RenderTargetView* destinationRTV,
		unsigned int FLAG_THREAD);
	virtual ~DownSampleRenderTask();
	
	void SetFullScreenQuad(Mesh* mesh);
	void SetFullScreenQuadInSlotInfo();

	void Execute();
private:
	ShaderResourceView* m_pSourceSRV = nullptr;
	const RenderTargetView* m_pDestinationRTV = nullptr;

	Mesh* m_pFullScreenQuadMesh = nullptr;

	SlotInfo m_Info;
	size_t m_NumIndices;
};

#endif
