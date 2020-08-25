#ifndef DEPTHSTENCILVIEW_H
#define DEPTHSTENCILVIEW_H

#include "DescriptorHeap.h"
#include "Resource.h"

class DepthStencilView
{
public:
	// Constructor that also creates a resource
	DepthStencilView(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring dsvResourceName,
		DescriptorHeap* descriptorHeap_DSV,
		D3D12_DEPTH_STENCIL_VIEW_DESC* depthStencilDescInput = nullptr);	// Set to nullptr to use default

	// Constructor that assigns a resource
	DepthStencilView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		Resource* resource,
		D3D12_DEPTH_STENCIL_VIEW_DESC* depthStencilDescInput = nullptr);	// Set to nullptr to use default

	virtual ~DepthStencilView();

	Resource* GetDSVResource() const;
	unsigned int GetDescriptorHeapIndex() const;

private:

	Resource* resource = nullptr;
	unsigned int descriptorHeapIndex_DSV = -1;

	void CreateResource(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring dsvResourceName);
	void CreateDSV(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		D3D12_DEPTH_STENCIL_VIEW_DESC* depthStencilDescInput);

	bool deleteResource = false;
};

#endif