#ifndef RENDERTARGETVIEW_H
#define RENDERTARGETVIEW_H

#include "View.h"
class RenderView;

#include <vector>

class RenderTargetView : public View
{
public:
	RenderTargetView(
		ID3D12Device5* device, 
		unsigned int width, unsigned int height,
		DescriptorHeap* descriptorHeap_RTV,
		D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc,
		Resource* resource, bool createRTVInConstructor);

	virtual ~RenderTargetView();

	void CreateRTV(ID3D12Device5* device, DescriptorHeap* descriptorHeap_RTV, D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc);

	// viewport & scizzorRect
	RenderView* GetRenderView() const;
private:
	// viewport & scizzorRect
	RenderView* m_pRenderView = nullptr;
};

#endif