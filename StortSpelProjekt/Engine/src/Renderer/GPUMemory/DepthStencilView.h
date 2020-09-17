#ifndef DEPTHSTENCILVIEW_H
#define DEPTHSTENCILVIEW_H

#include "View.h"

class DepthStencilView : public View
{
public:
	DepthStencilView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
		Resource* resource);

	virtual ~DepthStencilView();


	DXGI_FORMAT GetDXGIFormat() const;

private:
	DXGI_FORMAT m_DXGIFormat = DXGI_FORMAT_UNKNOWN;

	void createDepthStencilView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_DSV,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc);
};

#endif