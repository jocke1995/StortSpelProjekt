#include "stdafx.h"
#include "Bloom.h"

#include "RenderTarget.h"
#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"
#include "Resource.h"
#include "../Misc/Window.h"


Bloom::Bloom(
	ID3D12Device5* device,
	DescriptorHeap* dh_RTV,
	DescriptorHeap* dh_CBV_UAV_SRV,
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

	// A srv to read from in the compute shader (same resource as the rtv)
	createShaderResourceViews(device, dh_CBV_UAV_SRV);

	// A uav to write the blurred image to after gaussian blur
	createUnorderedAccessViews(device, dh_CBV_UAV_SRV, width, height);
}

Bloom::~Bloom()
{
	delete m_pRenderTarget;

	// Delete SRVs & UAVs
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		delete m_SRVs[i];
		delete m_UAVs[i];
	}
}

const RenderTarget* const Bloom::GetRenderTarget() const
{
	return m_pRenderTarget;
}

void Bloom::createBrightRenderTarget(ID3D12Device5* device5, DescriptorHeap* dhRTV, unsigned int width, unsigned int height)
{
	m_pRenderTarget = new RenderTarget(device5, width, height, dhRTV);
}

void Bloom::createShaderResourceViews(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;

	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		Resource* resource = m_pRenderTarget->GetResource(i);
		m_SRVs[i] = new ShaderResourceView(device, dh_CBV_UAV_SRV, &srvd, resource);
	}
}

void Bloom::createUnorderedAccessViews(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV, unsigned int width, unsigned int height)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_UAVs[i] = new UnorderedAccessView(device, dh_CBV_UAV_SRV, &uavDesc, width, height);
	}
}
