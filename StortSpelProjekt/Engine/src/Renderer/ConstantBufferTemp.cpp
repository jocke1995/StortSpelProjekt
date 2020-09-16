#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Resource.h"
#include "DescriptorHeap.h"

ConstantBufferTemp::ConstantBufferTemp(
	ID3D12Device5* device,
	unsigned int entrySize,
	std::wstring resourceName,
	unsigned int descriptorHeapIndex)
{
	unsigned int sizeAligned = (entrySize + 255) & ~255;
	m_pUploadResource = new Resource(device, sizeAligned, RESOURCE_TYPE::UPLOAD, resourceName);
	m_DescriptorHeapIndex = descriptorHeapIndex;
}

ConstantBufferTemp::~ConstantBufferTemp()
{
	delete m_pUploadResource;
}

Resource* ConstantBufferTemp::GetUploadResource() const
{
	return m_pUploadResource;
}

unsigned int ConstantBufferTemp::GetDescriptorHeapIndex() const
{
	return m_DescriptorHeapIndex;
}
