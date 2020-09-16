#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Resource.h"
#include "../DescriptorHeap.h"

ConstantBuffer::ConstantBuffer(ID3D12Device5* device, unsigned int entrySize, std::wstring resourceName, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	unsigned int sizeAligned = (entrySize + 255) & ~255;
	m_pUploadResource = new Resource(device, sizeAligned, RESOURCE_TYPE::UPLOAD, resourceName + L"_UPLOAD");
	m_pDefaultResource = new Resource(device, sizeAligned, RESOURCE_TYPE::DEFAULT, resourceName + L"_DEFAULT");
	m_pId = cbCounter++;
	CreateConstantBufferView(device, descriptorHeap_CBV_UAV_SRV);
}

bool ConstantBuffer::operator==(const ConstantBuffer& other)
{
	return m_pId == other.m_pId;
}

ConstantBuffer::~ConstantBuffer()
{
	delete m_pUploadResource;
	delete m_pDefaultResource;
}

Resource* ConstantBuffer::GetUploadResource() const
{
	return m_pUploadResource;
}

Resource* ConstantBuffer::GetDefaultResource() const
{
	return m_pDefaultResource;
}

unsigned int ConstantBuffer::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndex;
}

void ConstantBuffer::CreateConstantBufferView(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	m_DescriptorHeapIndex = descriptorHeap_CBV_UAV_SRV->GetNextDescriptorHeapIndex(1);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
	cbvd.BufferLocation = m_pDefaultResource->GetID3D12Resource1()->GetGPUVirtualAddress();
	cbvd.SizeInBytes = m_pDefaultResource->GetSize();
	device->CreateConstantBufferView(&cbvd, cdh);
}
