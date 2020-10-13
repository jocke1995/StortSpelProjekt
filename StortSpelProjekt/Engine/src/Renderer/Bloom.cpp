#include "stdafx.h"
#include "Bloom.h"

#include "GPUMemory/ShaderResourceView.h"
#include "GPUMemory/RenderTargetView.h"
#include "GPUMemory/Resource.h"
#include "PingPongResource.h"
#include "../Misc/Window.h"
#include "../Misc/Option.h"
#include "SwapChain.h"


Bloom::Bloom(
	ID3D12Device5* device,
	DescriptorHeap* dh_RTV,
	DescriptorHeap* dh_CBV_UAV_SRV,
	SwapChain* swapChain)
{
	UINT resolutionWidth = 0;
	UINT resolutionHeight = 0;
	swapChain->GetDX12SwapChain()->GetSourceSize(&resolutionWidth, &resolutionHeight);

	// A resource, rtv and srv for "bright" areas on the screen
	createBrightTuple(device, dh_RTV, dh_CBV_UAV_SRV, resolutionWidth, resolutionHeight);

	// Create the pingpongBuffers where the starting point to read from will be in the "brightTuple"
	std::wstring resourceName = L"PingPongBuffer";
	m_Resources[0] = createResource(
		device,
		m_BlurWidth, m_BlurHeight,
		resourceName + std::to_wstring(0),
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_Resources[1] = createResource(
		device,
		m_BlurWidth, m_BlurHeight,
		resourceName + std::to_wstring(0),
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

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

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_PingPongResources[0]->CreateRTV(device, m_BlurWidth, m_BlurHeight, dh_RTV, &rtvDesc);
}

Bloom::~Bloom()
{
	for (unsigned int i = 0; i < 2; i++)
	{
		delete m_Resources[i];
		delete m_PingPongResources[i];
	}

	delete std::get<0>(m_BrightTuple);
	delete std::get<1>(m_BrightTuple);
	delete std::get<2>(m_BrightTuple);
}

unsigned int Bloom::GetBlurWidth() const
{
	return m_BlurWidth;
}

unsigned int Bloom::GetBlurHeight() const
{
	return m_BlurHeight;
}

const std::tuple<Resource*, RenderTargetView*, ShaderResourceView*>* Bloom::GetBrightTuple() const
{
	return &m_BrightTuple;
}

const PingPongResource* Bloom::GetPingPongResource(unsigned int index) const
{
	return m_PingPongResources[index];
}

Resource* Bloom::createResource(
	ID3D12Device5* device,
	unsigned int width, unsigned int height,
	std::wstring resourceName,
	D3D12_RESOURCE_FLAGS flags,
	D3D12_RESOURCE_STATES startState)
{
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = flags;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = resourceDesc.Format;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

	return new Resource(device, &resourceDesc, &clearValue, resourceName, startState);
}

void Bloom::createBrightTuple(
	ID3D12Device5* device5,
	DescriptorHeap* dhRTV,
	DescriptorHeap* dh_CBV_UAV_SRV,
	unsigned int width, unsigned int height)
{
	// Resource
	Resource* resource = createResource(
		device5,
		width, height,
		L"BrightResource",
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	RenderTargetView* rtv = new RenderTargetView(device5, width, height, dhRTV,  &rtvDesc, resource, true);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	ShaderResourceView* srv = new ShaderResourceView(device5, dh_CBV_UAV_SRV, &srvDesc, resource);

	std::get<0>(m_BrightTuple) = resource;
	std::get<1>(m_BrightTuple) = rtv;
	std::get<2>(m_BrightTuple) = srv;
}
