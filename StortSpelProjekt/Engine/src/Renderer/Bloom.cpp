#include "stdafx.h"
#include "Bloom.h"

#include "RenderTarget.h"
#include "ShaderResourceView.h"
#include "Resource.h"
#include "../Misc/Window.h"


Bloom::Bloom(
	ID3D12Device5* device,
	DescriptorHeap* dh_RTV,
	DescriptorHeap* dh_CBV_UAV_SRV,
	const HWND* hwnd)
{
	createBrightRenderTarget(device, dh_RTV, hwnd);
	createShaderResourceView(device, dh_CBV_UAV_SRV);
}

Bloom::~Bloom()
{
	delete m_pRenderTarget;

	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		delete m_SRVs[i];
	}
}

const RenderTarget* const Bloom::GetRenderTarget() const
{
	return m_pRenderTarget;
}

void Bloom::createBrightRenderTarget(ID3D12Device5* device5, DescriptorHeap* dhRTV, const HWND* hwnd)
{
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	m_pRenderTarget = new RenderTarget(device5, width, height, dhRTV);
}

void Bloom::createShaderResourceView(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;

	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		Resource* resource = m_pRenderTarget->GetResource(i);
		m_SRVs[i] = new ShaderResourceView(device, dh_CBV_UAV_SRV, nullptr, resource);
	}
}
