#include "stdafx.h"
#include "Bloom.h"

#include "RenderTarget.h"
#include "Resource.h"
#include "../Misc/Window.h"


BloomResources::BloomResources(
	ID3D12Device5* device,
	DescriptorHeap* dh_RTV,
	const HWND* hwnd)
{
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	// A renderTarget for "bright" areas on the screen
	createBrightRenderTarget(device, dh_RTV, width, height);

	// two resources (which will be interpreted as UAVs) to write the blurred image to after gaussian blur
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	m_pResourceToWrite = new Resource(device, &resourceDesc, nullptr, L"ResourceToBlur", D3D12_RESOURCE_STATE_GENERIC_READ);
}

BloomResources::~BloomResources()
{
	delete m_pRenderTarget;

	// Delete the resource
	delete m_pResourceToWrite;
}

const RenderTarget* const BloomResources::GetRenderTarget() const
{
	return m_pRenderTarget;
}

const Resource* const BloomResources::GetResourceToWrite() const
{
	return m_pResourceToWrite;
}

void BloomResources::createBrightRenderTarget(ID3D12Device5* device5, DescriptorHeap* dhRTV, unsigned int width, unsigned int height)
{
	m_pRenderTarget = new RenderTarget(device5, width, height, dhRTV, 1);
}
