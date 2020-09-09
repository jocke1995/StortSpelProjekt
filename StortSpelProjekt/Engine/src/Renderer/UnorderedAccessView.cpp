#include "stdafx.h"
#include "UnorderedAccessView.h"

#include "Resource.h"

#include "DescriptorHeap.h"
#include "Resource.h"

UnorderedAccessView::UnorderedAccessView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc,
	unsigned int width, unsigned int height)
{
	// Create new Resources
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = width; // width of the texture
	resourceDesc.Height = height; // height of the texture
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	m_pResource = new Resource(device, &resourceDesc, nullptr, L"Blur_UAV_RESOURCE");

	createUnorderedAccessView(device, descriptorHeap_CBV_UAV_SRV, uavDesc);
}

UnorderedAccessView::~UnorderedAccessView()
{
	delete m_pResource;
}

unsigned int UnorderedAccessView::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndex;
}

void UnorderedAccessView::createUnorderedAccessView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
{
	m_DescriptorHeapIndex = descriptorHeap_CBV_UAV_SRV->GetNextDescriptorHeapIndex(1);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);

	device->CreateUnorderedAccessView(m_pResource->GetID3D12Resource1(), nullptr, desc, cdh);
}
