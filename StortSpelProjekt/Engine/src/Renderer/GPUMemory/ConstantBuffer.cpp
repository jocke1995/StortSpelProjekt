#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Resource.h"
#include "ConstantBufferView.h"
#include "../DescriptorHeap.h"

ConstantBuffer::ConstantBuffer(
	ID3D12Device5* device,
	unsigned int entrySize,
	std::wstring resourceName,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	unsigned int sizeAligned = (entrySize + 255) & ~255;
	m_pUploadResource = new Resource(device, sizeAligned, RESOURCE_TYPE::UPLOAD, resourceName + L"_UPLOAD");
	m_pDefaultResource = new Resource(device, sizeAligned, RESOURCE_TYPE::DEFAULT, resourceName + L"_DEFAULT");

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_pDefaultResource->GetID3D12Resource1()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_pDefaultResource->GetSize();

	m_pCBV = new ConstantBufferView(device, descriptorHeap_CBV_UAV_SRV, &cbvDesc, m_pDefaultResource);

	m_Id = s_CbCounter++;
}

bool ConstantBuffer::operator==(const ConstantBuffer& other)
{
	return m_Id == other.m_Id;
}

ConstantBuffer::~ConstantBuffer()
{
	delete m_pUploadResource;
	delete m_pDefaultResource;
	delete m_pCBV;
}

Resource* ConstantBuffer::GetUploadResource() const
{
	return m_pUploadResource;
}

Resource* ConstantBuffer::GetDefaultResource() const
{
	return m_pDefaultResource;
}

const ConstantBufferView* const ConstantBuffer::GetCBV() const
{
	return m_pCBV;
}
