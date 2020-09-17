#ifndef DEPTHSTENCILVIEW_H
#define DEPTHSTENCILVIEW_H

class DescriptorHeap;
class Resource;

class DepthStencilView
{
public:
	DepthStencilView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
		Resource* resource);

	virtual ~DepthStencilView();

	Resource* GetDSVResource() const;
	unsigned int GetDescriptorHeapIndex() const;

	DXGI_FORMAT GetDXGIFormat() const;

private:
	Resource* m_pResource = nullptr;
	unsigned int m_DescriptorHeapIndexDSV = -1;
	DXGI_FORMAT m_DXGIFormat = DXGI_FORMAT_UNKNOWN;

	void createDepthStencilView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc);
}; // format = DXGI_FORMAT_D32_FLOAT

#endif