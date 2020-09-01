#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Resource.h"
#include "DescriptorHeap.h"

ConstantBuffer::ConstantBuffer(
	ID3D12Device5* device,
	unsigned int entrySize,
	std::wstring resourceName,
	unsigned int descriptorHeapIndex)
{
	unsigned int sizeAligned = (entrySize + 255) & ~255;
	this->m_pUploadResource = new Resource(device, sizeAligned, RESOURCE_TYPE::UPLOAD, resourceName);
	this->m_DescriptorHeapIndex = descriptorHeapIndex;
}

ConstantBuffer::~ConstantBuffer()
{
	delete this->m_pUploadResource;
}

Resource* ConstantBuffer::GetUploadResource() const
{
	return this->m_pUploadResource;
}

unsigned int ConstantBuffer::GetDescriptorHeapIndex() const
{
	return this->m_DescriptorHeapIndex;
}
