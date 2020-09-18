#include "stdafx.h"
#include "UnorderedAccessView.h"

#include "../DescriptorHeap.h"
#include "Resource.h"

UnorderedAccessView::UnorderedAccessView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc,
	Resource* resource)
	:View(descriptorHeap_CBV_UAV_SRV, resource)
{
	createUnorderedAccessView(device, descriptorHeap_CBV_UAV_SRV, uavDesc);
}

UnorderedAccessView::~UnorderedAccessView()
{
}

void UnorderedAccessView::createUnorderedAccessView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);
	device->CreateUnorderedAccessView(m_pResource->GetID3D12Resource1(), nullptr, desc, cdh);
}
