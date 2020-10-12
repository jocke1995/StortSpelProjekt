#include "stdafx.h"
#include "PingPongResource.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/ShaderResourceView.h"
#include "GPUMemory/UnorderedAccessView.h"
#include "GPUMemory/RenderTargetView.h"

PingPongResource::PingPongResource(
	Resource* resource,
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
	D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
	m_pResource = resource;
	m_pSRV = new ShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, srvDesc, resource);
	m_pUAV = new UnorderedAccessView(device, descriptorHeap_CBV_UAV_SRV, uavDesc, resource);
}

PingPongResource::~PingPongResource()
{
	delete m_pSRV;
	delete m_pUAV;

	if (m_pRTV != nullptr)
	{
		delete m_pRTV;
	}
}

const Resource* const PingPongResource::GetResource() const
{
	return m_pResource;
}

const ShaderResourceView* const PingPongResource::GetSRV() const
{
	return m_pSRV;
}

const UnorderedAccessView* const PingPongResource::GetUAV() const
{
	return m_pUAV;
}

const RenderTargetView* const PingPongResource::GetRTV() const
{
	return m_pRTV;
}

void PingPongResource::CreateRTV(ID3D12Device5* device, unsigned int width, unsigned int height, DescriptorHeap* dhRTV, D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc)
{
	if (m_pRTV == nullptr)
	{
		m_pRTV = new RenderTargetView(device, width, height, dhRTV, rtvDesc, m_pResource, true);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Trying to CreateRTV in 'PingPongResource' when it already exists\n");
	}
}
