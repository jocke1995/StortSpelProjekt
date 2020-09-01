#ifndef DEPTHSTENCILVIEW_H
#define DEPTHSTENCILVIEW_H

class DescriptorHeap;
class Resource;

// DX12
#include <dxgiformat.h>

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
	Resource* m_pResource = nullptr;
	unsigned int m_DescriptorHeapIndexDSV = -1;
	DXGI_FORMAT m_DXGIFormat = DXGI_FORMAT_UNKNOWN;
	bool m_DeleteResource = false;

	void createResource(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring dsvResourceName,
		DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);

	void createDSV(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);
};

#endif