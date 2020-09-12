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
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	virtual ~MergeRenderTask();

	void SetFullScreenQuad(Mesh* mesh);

	void Execute();
private:
	Mesh* m_pFullScreenQuadMesh = nullptr;

};

#endif
