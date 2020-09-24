#include "stdafx.h"
#include "Bloom.h"

#include "GPUMemory/RenderTargetView.h"
#include "GPUMemory/Resource.h"
#include "PingPongResource.h"
#include "../Misc/Window.h"
#include "../Misc/Option.h"


Bloom::Bloom(
	ID3D12Device5* device,
	DescriptorHeap* dh_RTV,
	DescriptorHeap* dh_CBV_UAV_SRV)
{
	int width = Option::GetInstance().GetVariable("resolutionWidth");
	int height = Option::GetInstance().GetVariable("resolutionHeight");

	createResources(device, width, height);

	// A renderTarget for "bright" areas on the screen
	createBrightRenderTarget(device, dh_RTV, width, height);

	// Create the pingpongBuffers where index 0 will be the starting point to read from.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	
	for (unsigned int i = 0; i < 2; i++)
	{
		m_PingPongResources[i] = new PingPongResource(m_Resources[i], device, dh_CBV_UAV_SRV, &srvDesc, &uavDesc);
	}
}

Bloom::~Bloom()
{
	for (unsigned int i = 0; i < 2; i++)
	{
		delete m_Resources[i];
		delete m_PingPongResources[i];
	}
	delete m_pRenderTargetView;
}

const RenderTargetView* const Bloom::GetRenderTargetView() const
{
	return m_pRenderTargetView;
}

const PingPongResource* Bloom::GetPingPongResource(unsigned int index) const
{
	return m_PingPongResources[index];
}

void Bloom::createResources(ID3D12Device5* device, unsigned int width, unsigned int height)
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

	m_Resources[0] = new Resource(device, &resourceDesc, &clearValue, L"Bloom0_RESOURCE", D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_Resources[1] = new Resource(device, &resourceDesc, &clearValue, L"Bloom1_RESOURCE", D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void Bloom::createBrightRenderTarget(
	ID3D12Device5* device5,
	DescriptorHeap* dhRTV,
	unsigned int width, unsigned int height)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// Resource 0 will be used as the starting resource to read from during the blurring pass.
	m_pRenderTargetView = new RenderTargetView(device5, width, height, dhRTV,  &rtvDesc, m_Resources[0], true);
}
