#ifndef PREDEPTHRENDERTASK_H
#define PREDEPTHRENDERTASK_H

#include "RenderTask.h"

class PreDepthRenderTask : public RenderTask
{
public:
	PreDepthRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~PreDepthRenderTask();

	void Execute();

private:
	void drawRenderComponent(
		component::ModelComponent* mc,
		component::TransformComponent* tc,
		const DirectX::XMMATRIX* viewProjTransposed,
		ID3D12GraphicsCommandList5* cl);
};

#endif