#ifndef PROGRESSBARRENDERTASK_H
#define PROGRESSBARRENDERTASK_H

#include "RenderTask.h"

class RootSignature;

class ConstantBufferView;
class Resource;

namespace component
{
	class ProgressBarComponent;
}

class ProgressBarRenderTask : public RenderTask
{
public:
	ProgressBarRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	~ProgressBarRenderTask();

	void Execute();

	void SetBillboardMesh(Mesh* quadMesh);
	void SetProgressBarComponents(std::vector<component::ProgressBarComponent*>* progressBarComponents);

private:
	Mesh* m_pQuad = nullptr;

	std::vector<component::ProgressBarComponent*> m_ProgressBarComponents;
};

#endif