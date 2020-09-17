#ifndef RENDERTARGETVIEW_H
#define RENDERTARGETVIEW_H

class DescriptorHeap;
class Resource;
class RenderView;

#include <vector>

class RenderTargetView
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

	Resource* GetResource() const;
	// viewport & scizzorRect
	RenderView* GetRenderView() const;
	const unsigned int GetDescriptorHeapIndex() const;
private:
	Resource* m_pResource;
	unsigned int m_DescriptorHeapIndex;


	// viewport & scizzorRect
	RenderView* m_pRenderView = nullptr;
};

#endif