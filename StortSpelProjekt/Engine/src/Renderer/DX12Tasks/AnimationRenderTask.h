#ifndef ANIMATIONRENDERTASK_H
#define ANIMATIONRENDERTASK_H

#include "RenderTask.h"

class AnimationRenderTask : public RenderTask
{
public:
	AnimationRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature, 
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	~AnimationRenderTask();

	void Execute();

private:
	void drawRenderComponent(
		component::ModelComponent* mc,
		component::TransformComponent* tc,
		const DirectX::XMMATRIX* viewProjTransposed,
		ID3D12GraphicsCommandList5* cl);
};

#endif