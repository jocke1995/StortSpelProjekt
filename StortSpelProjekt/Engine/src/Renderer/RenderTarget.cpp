#include "stdafx.h"
#include "RenderTarget.h"

#include "DescriptorHeap.h"
#include "Resource.h"
#include "RenderView.h"

RenderTarget::RenderTarget(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_RTV,
	Resource* resource,
	unsigned int width, unsigned int height)
{
	m_pResource = resource;
	m_dhIndex = descriptorHeap_RTV->GetNextDescriptorHeapIndex(1);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_RTV->GetCPUHeapAt(m_dhIndex);
	device->CreateRenderTargetView(resource->GetID3D12Resource1(), &rtvDesc, cdh);

	// RenderView
	m_pRenderView = new RenderView(width, height);
}

RenderTarget::RenderTarget(unsigned int width, unsigned int height, DescriptorHeap* descriptorHeap_RTV, Resource* resource)
{
	m_dhIndex = descriptorHeap_RTV->GetNextDescriptorHeapIndex(1);
	m_pResource = resource;

	m_pRenderView = new RenderView(width, height);
}

RenderTarget::~RenderTarget()
{
	delete m_pRenderView;
}

Resource* RenderTarget::GetResource() const
{
	return m_pResource;
}

RenderView* RenderTarget::GetRenderView() const
{
	return m_pRenderView;
}

const unsigned int RenderTarget::GetDescriptorHeapIndex() const
{
	return m_dhIndex;
}
