#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(
	ID3D12Device5* device,
	unsigned int entrySize,
	std::wstring resourceName,
	unsigned int descriptorHeapIndex)
{
	this->uploadResource = new Resource(device, entrySize, RESOURCE_TYPE::UPLOAD, resourceName);
	this->descriptorHeapIndex = descriptorHeapIndex;
}

ConstantBuffer::~ConstantBuffer()
{
	delete this->uploadResource;
}

Resource* ConstantBuffer::GetUploadResource() const
{
	return this->uploadResource;
}

unsigned int ConstantBuffer::GetDescriptorHeapIndex() const
{
	return this->descriptorHeapIndex;
}
