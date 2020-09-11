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

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	clearValue.Color[0] = 0.1f;
	clearValue.Color[1] = 0.1f;
	clearValue.Color[2] = 0.1f;
	clearValue.Color[3] = 1.0f;

	m_pResource = new Resource(device, &resourceDesc, &clearValue, L"Bloom0_RESOURCE", D3D12_RESOURCE_STATE_GENERIC_READ);

	// A renderTarget for "bright" areas on the screen
	createBrightRenderTarget(device, dh_RTV, width, height);
}

BloomResources::~BloomResources()
{
	delete m_pResource;
	delete m_pRenderTarget;
}

const RenderTarget* const BloomResources::GetRenderTarget() const
{
	return m_pRenderTarget;
}

const PingPongBuffer* BloomResources::GetPingPongBuffer(unsigned int index) const
{
	return m_PingPongBuffers[index];
}

void BloomResources::createBrightRenderTarget(
	ID3D12Device5* device5,
	DescriptorHeap* dhRTV,
	unsigned int width, unsigned int height)
{
	m_pRenderTarget = new RenderTarget(device5, dhRTV, m_pResource, width, height);
}
