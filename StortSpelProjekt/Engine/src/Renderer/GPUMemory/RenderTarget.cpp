#include "stdafx.h"
#include "RenderTarget.h"

#include "Resource.h"
#include "RenderTargetView.h"
#include "../DescriptorHeap.h"

RenderTarget::RenderTarget(
	ID3D12Device5* device,
	unsigned int width, unsigned int height,
	std::wstring resourceName,
	DescriptorHeap* descriptorHeap_RTV)
{
	createResource(device, width, height, resourceName);
	
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	m_pRTV = new RenderTargetView(device, width, height, descriptorHeap_RTV, &rtvDesc, m_pResource, true);
}

bool RenderTarget::operator==(const RenderTarget& other)
{
	return m_pId == other.m_pId;
}

RenderTarget::~RenderTarget()
{
	delete m_pResource;
	delete m_pRTV;
}

const Resource* const RenderTarget::GetDefaultResource() const
{
	return m_pResource;
}

const RenderTargetView* const RenderTarget::GetRTV() const
{
	return m_pRTV;
}

void RenderTarget::createResource(ID3D12Device5* device, unsigned int width, unsigned int height, std::wstring resourceName)
{
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = resourceDesc.Format;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

	m_pResource = new Resource(device, &resourceDesc, &clearValue, L"Bloom0_RESOURCE", D3D12_RESOURCE_STATE_RENDER_TARGET);
}
