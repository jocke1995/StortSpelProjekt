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
		DescriptorHeap* descriptorHeap_RTV);

	// RenderTarget without creating a comittedResource
	RenderTarget(unsigned int width, unsigned int height);
	virtual ~RenderTarget();

	Resource* GetResource(unsigned int index) const;
	// viewport & scizzorRect
	RenderView* GetRenderView() const;
protected:
	std::vector<Resource*> m_Resources;
	// viewport & scizzorRect
	RenderView* m_pRenderView = nullptr;
};

#endif