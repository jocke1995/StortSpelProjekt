#include "stdafx.h"
#include "ShaderResourceView.h"

#include "../DescriptorHeap.h"
#include "Resource.h"

ShaderResourceView::ShaderResourceView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
	Resource* resource)
	:View(descriptorHeap_CBV_UAV_SRV, resource)
{
	createShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, srvDesc);
}

ShaderResourceView::~ShaderResourceView()
{
	
}

void ShaderResourceView::createShaderResourceView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);
	device->CreateShaderResourceView(m_pResource->GetID3D12Resource1(), srvDesc, cdh);
}
