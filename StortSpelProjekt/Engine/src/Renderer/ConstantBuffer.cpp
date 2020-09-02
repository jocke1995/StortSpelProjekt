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
	m_pUploadResource = new Resource(device, sizeAligned, RESOURCE_TYPE::UPLOAD, resourceName);
	m_DescriptorHeapIndex = descriptorHeapIndex;
}

ConstantBuffer::~ConstantBuffer()
{
	delete m_pUploadResource;
}

Resource* ConstantBuffer::GetUploadResource() const
{
	return m_pUploadResource;
}

unsigned int ConstantBuffer::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndex;
}
