#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "DescriptorHeap.h"
#include "Resource.h"
#include "RenderView.h"

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
	std::vector<Resource*> resources;

	// viewport & scizzorRect
	RenderView* renderView = nullptr;
};

#endif