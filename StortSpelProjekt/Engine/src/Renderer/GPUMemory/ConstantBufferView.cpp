#include "stdafx.h"
#include "ConstantBufferView.h"

#include "../DescriptorHeap.h"
#include "Resource.h"

ConstantBufferView::ConstantBufferView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc,
	Resource* resource)
{
	m_pResource = resource;
	m_DescriptorHeapIndex = descriptorHeap_CBV_UAV_SRV->GetNextDescriptorHeapIndex(1);

	createConstantBufferView(device, descriptorHeap_CBV_UAV_SRV, cbvDesc);
}

ConstantBufferView::~ConstantBufferView()
{

}

unsigned int ConstantBufferView::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndex;
}

const Resource* const ConstantBufferView::GetResource() const
{
	return m_pResource;
}

void ConstantBufferView::createConstantBufferView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);

	device->CreateConstantBufferView(cbvDesc, cdh);
}
