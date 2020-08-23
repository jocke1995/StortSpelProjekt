#include "ShaderResourceView.h"

ShaderResourceView::ShaderResourceView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
	Resource* resource)
{
	this->descriptorHeapIndex = descriptorHeap_CBV_UAV_SRV->GetNextDescriptorHeapIndex(1);

	this->CreateShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, desc, resource);
}

ShaderResourceView::~ShaderResourceView()
{
	
}

unsigned int ShaderResourceView::GetDescriptorHeapIndex() const
{
	return this->descriptorHeapIndex;
}

void ShaderResourceView::CreateShaderResourceView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
	Resource* resource)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(this->descriptorHeapIndex);

	device->CreateShaderResourceView(resource->GetID3D12Resource1(), desc, cdh);
}