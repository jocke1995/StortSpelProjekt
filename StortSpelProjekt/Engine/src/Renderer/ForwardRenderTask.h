#ifndef FORWARDRENDERTASK_H
#define FORWARDRENDERTASK_H

#include "RenderTask.h"

class FowardRenderTask : public RenderTask
{
public:
	FowardRenderTask(ID3D12Device5* device, 
		RootSignature* rootSignature, 
		LPCWSTR VSName, LPCWSTR PSName, 
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
		LPCTSTR psoName);
	~FowardRenderTask();

	void Execute();

private:
	void DrawRenderComponent(
		component::MeshComponent* mc,
		component::TransformComponent* tc,
		const DirectX::XMMATRIX* viewProjTransposed,
		ID3D12GraphicsCommandList5* cl);
};

#endif