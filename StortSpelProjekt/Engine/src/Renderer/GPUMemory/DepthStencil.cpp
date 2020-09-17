#include "stdafx.h"
#include "DepthStencil.h"

#include "Resource.h"
#include "DepthStencilView.h"
#include "../DescriptorHeap.h"

DepthStencil::DepthStencil(
    ID3D12Device5* device,
	unsigned int width,
	unsigned int height,
    std::wstring resourceName,
    D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
    DescriptorHeap* descriptorHeap_DSV)
{
    createResource(device, width, height, resourceName, dsvDesc->Format);

	m_pDSV = new DepthStencilView(device, descriptorHeap_DSV, dsvDesc, m_pDefaultResource);

    m_Id = s_DsCounter++;
}

bool DepthStencil::operator==(const DepthStencil& other)
{
    return m_Id == other.m_Id;
}

DepthStencil::~DepthStencil()
{
    delete m_pDefaultResource;
    delete m_pDSV;
}

const Resource* const DepthStencil::GetDefaultResource() const
{
    return m_pDefaultResource;
}

const DepthStencilView* const DepthStencil::GetDSV() const
{
    return m_pDSV;
}

void DepthStencil::createResource(ID3D12Device5* device, unsigned int width, unsigned int height, std::wstring resourceName, DXGI_FORMAT format)
{
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Format = format;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.DepthStencil.Depth = 1;
	clearValue.DepthStencil.Stencil = 0;

	m_pDefaultResource = new Resource(
		device,
		&resourceDesc,
		&clearValue,
		resourceName,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
}
