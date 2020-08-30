#ifndef DEPTHSTENCILVIEW_H
#define DEPTHSTENCILVIEW_H

#include "DescriptorHeap.h"
#include "Resource.h"

class DepthStencilView
{
public:
	// Constructor that creates a resource
	DepthStencilView(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring dsvResourceName,
		DescriptorHeap* descriptorHeap_DSV,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	// Constructor that assigns a resource
	DepthStencilView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		Resource* resource,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);	// Set to nullptr to use default

	virtual ~DepthStencilView();

	Resource* GetDSVResource() const;
	unsigned int GetDescriptorHeapIndex() const;
	DXGI_FORMAT GetDXGIFormat() const;

private:

	Resource* resource = nullptr;
	unsigned int descriptorHeapIndex_DSV = -1;
	DXGI_FORMAT dxgi_Format = DXGI_FORMAT_UNKNOWN;

	void CreateResource(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring dsvResourceName,
		DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);

	void CreateDSV(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);

	bool deleteResource = false;
};

#endif