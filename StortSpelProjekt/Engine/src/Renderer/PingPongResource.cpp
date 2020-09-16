#include "stdafx.h"
#include "PingPongResource.h"

#include "GPUMemory/Resource.h"
#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

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
