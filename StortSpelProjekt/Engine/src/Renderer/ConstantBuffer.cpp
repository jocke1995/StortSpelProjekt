#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Resource.h"
#include "DescriptorHeap.h"

ConstantBuffer::ConstantBuffer(ID3D12Device5* device, unsigned int entrySize, std::wstring defaultName, unsigned int descriptorHeapIndex, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
	:ConstantBufferTemp(device, entrySize, L"ConstantBuffer_UPLOAD_RESOURCE", descriptorHeapIndex)
{
	unsigned int sizeAligned = (entrySize + 255) & ~255;
	m_pId = cbCounter++;
	m_pDefaultResource = new Resource(device, sizeAligned, RESOURCE_TYPE::DEFAULT, defaultName);
	CreateConstantBufferView(device, descriptorHeap_CBV_UAV_SRV);
}

bool ConstantBuffer::operator==(const ConstantBuffer& other)
{
	return m_pId == other.m_pId;
}

ConstantBuffer::~ConstantBuffer()
{
	delete m_pDefaultResource;
}

Resource* ConstantBuffer::GetCBVResource() const
{
	return m_pDefaultResource;
}

void ConstantBuffer::CreateConstantBufferView(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
	cbvd.BufferLocation = m_pDefaultResource->GetID3D12Resource1()->GetGPUVirtualAddress();
	cbvd.SizeInBytes = m_pDefaultResource->GetSize();
	device->CreateConstantBufferView(&cbvd, cdh);
}
