#ifndef MERGERENDERTASK_H
#define MERGERENDERTASK_H

#include "RenderTask.h"
class Mesh;

class MergeRenderTask : public RenderTask
{
public:
	MergeRenderTask(
		ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	virtual ~MergeRenderTask();
	
	void AddSRVIndexToMerge(unsigned int srvIndex);
	void SetFullScreenQuad(Mesh* mesh);
	void CreateSlotInfo();

	void Execute();
private:
	Mesh* m_pFullScreenQuadMesh = nullptr;
	std::vector<unsigned int> m_SRVIndices;

	SlotInfo m_Info;
	size_t m_NumIndices;

};

#endif
