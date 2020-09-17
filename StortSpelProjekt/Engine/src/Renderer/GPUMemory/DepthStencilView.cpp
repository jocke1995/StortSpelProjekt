#include "stdafx.h"
#include "DepthStencilView.h"

#include "Resource.h"
#include "../DescriptorHeap.h"

DepthStencilView::DepthStencilView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_DSV,
	D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
	Resource* resource)
{
	m_pResource = resource;
	m_DXGIFormat = dsvDesc->Format;
	createDepthStencilView(device, descriptorHeap_DSV, dsvDesc);
}

DepthStencilView::~DepthStencilView()
{
}

Resource* DepthStencilView::GetDSVResource() const
{
	return m_pResource;
}

unsigned int DepthStencilView::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndexDSV;
}

DXGI_FORMAT DepthStencilView::GetDXGIFormat() const
{
	return m_DXGIFormat;
}

void DepthStencilView::createDepthStencilView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_DSV,
	D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc)
{
	m_DescriptorHeapIndexDSV = descriptorHeap_DSV->GetNextDescriptorHeapIndex(1);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_DSV->GetCPUHeapAt(m_DescriptorHeapIndexDSV);
	device->CreateDepthStencilView(m_pResource->GetID3D12Resource1(), dsvDesc, cdh);
}
