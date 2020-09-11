#ifndef RENDERTARGET_H
#define RENDERTARGET_H

class DescriptorHeap;
class Resource;
class RenderView;

#include <vector>

class RenderTarget
{
public:
	// RenderTarget which creates a comitted Resource
	RenderTarget(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		DescriptorHeap* descriptorHeap_RTV,
		unsigned int numRenderTargets);

	// RenderTarget without creating a comittedResource
	RenderTarget(unsigned int width, unsigned int height, unsigned int numRenderTargets);
	virtual ~RenderTarget();

	Resource* GetResource(unsigned int index) const;
	// viewport & scizzorRect
	RenderView* GetRenderView() const;
	const unsigned int GetDescriptorHeapIndex(unsigned int backBufferIndex) const;
protected:
	std::vector<Resource*> m_Resources;
	std::array<unsigned int, NUM_SWAP_BUFFERS> m_dhIndices;	// One descriptorheap index for each "rtv buffer".
	// viewport & scizzorRect
	RenderView* m_pRenderView = nullptr;
};

#endif