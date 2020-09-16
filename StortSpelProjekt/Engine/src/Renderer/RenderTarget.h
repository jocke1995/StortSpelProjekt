#ifndef RENDERTARGET_H
#define RENDERTARGET_H

class DescriptorHeap;
class Resource;
class RenderView;

#include <vector>

class RenderTarget
{
public:
	// RenderTarget which creates a rtv on the given resource
	RenderTarget(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_RTV,
		Resource* resource,
		unsigned int width, unsigned int height);

	// RenderTarget without creating a comittedResource (swapchain uses this constructor)
	RenderTarget(unsigned int width, unsigned int height, DescriptorHeap* descriptorHeap_RTV, Resource* resource);
	virtual ~RenderTarget();

	Resource* GetResource() const;
	// viewport & scizzorRect
	RenderView* GetRenderView() const;
	const unsigned int GetDescriptorHeapIndex() const;
protected:
	Resource* m_pResource;
	unsigned int m_dhIndex;	// One descriptorheap index for each "rtv buffer".
	// viewport & scizzorRect
	RenderView* m_pRenderView = nullptr;
};

#endif