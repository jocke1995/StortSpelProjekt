#include "stdafx.h"
#include "ConstantBufferView.h"

#include "../DescriptorHeap.h"
#include "Resource.h"

ConstantBufferView::ConstantBufferView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc,
	Resource* resource)
	:View(descriptorHeap_CBV_UAV_SRV, resource)
{
	createConstantBufferView(device, descriptorHeap_CBV_UAV_SRV, cbvDesc);
}

ConstantBufferView::~ConstantBufferView()
{

}

void ConstantBufferView::createConstantBufferView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);
	device->CreateConstantBufferView(cbvDesc, cdh);
}
