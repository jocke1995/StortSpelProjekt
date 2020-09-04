#include "stdafx.h"
#include "ShaderResourceView.h"

#include "DescriptorHeap.h"
#include "Resource.h"

ShaderResourceView::ShaderResourceView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
	Resource* resource)
{
	m_DescriptorHeapIndex = descriptorHeap_CBV_UAV_SRV->GetNextDescriptorHeapIndex(1);

	createShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, desc, resource);
}

ShaderResourceView::~ShaderResourceView()
{
	
}

unsigned int ShaderResourceView::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndex;
}

void ShaderResourceView::createShaderResourceView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
	Resource* resource)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);

	device->CreateShaderResourceView(resource->GetID3D12Resource1(), desc, cdh);
}