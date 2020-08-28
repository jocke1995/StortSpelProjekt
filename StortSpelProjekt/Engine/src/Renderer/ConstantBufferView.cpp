#include "ConstantBufferView.h"

ConstantBufferView::ConstantBufferView(ID3D12Device5* device, unsigned int entrySize, std::wstring defaultName, unsigned int descriptorHeapIndex, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
	:ConstantBuffer(device, entrySize, L"ConstantBuffer_UPLOAD_RESOURCE", descriptorHeapIndex)
{
	this->id = cbvCounter++;
	this->defaultResource = new Resource(device, entrySize, RESOURCE_TYPE::DEFAULT, defaultName);
	this->CreateConstantBufferView(device, descriptorHeap_CBV_UAV_SRV);
}

bool ConstantBufferView::operator==(const ConstantBufferView& other)
{
	return this->id == other.id;
}

ConstantBufferView::~ConstantBufferView()
{
	delete this->defaultResource;
}

Resource* ConstantBufferView::GetCBVResource() const
{
	return this->defaultResource;
}

void ConstantBufferView::CreateConstantBufferView(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(this->descriptorHeapIndex);
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
	cbvd.BufferLocation = this->defaultResource->GetID3D12Resource1()->GetGPUVirtualAddress();
	cbvd.SizeInBytes = this->defaultResource->GetSize();
	device->CreateConstantBufferView(&cbvd, cdh);
}
