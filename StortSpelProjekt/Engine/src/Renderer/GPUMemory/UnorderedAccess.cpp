#include "stdafx.h"
#include "UnorderedAccess.h"

#include "Resource.h"
#include "UnorderedAccessView.h"
#include "../DescriptorHeap.h"

UnorderedAccess::UnorderedAccess(
	ID3D12Device5* device,
	unsigned int entrySize,
	std::wstring resourceName,
	D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	m_pUploadResource = new Resource(device, entrySize, RESOURCE_TYPE::UPLOAD, resourceName + L"_UPLOAD");
	m_pDefaultResource = new Resource(device, entrySize, RESOURCE_TYPE::DEFAULT, resourceName + L"_DEFAULT");
	m_pUAV = new UnorderedAccessView(device, descriptorHeap_CBV_UAV_SRV, uavDesc, m_pDefaultResource);

	m_Id = s_UaCounter++;
}

bool UnorderedAccess::operator==(const UnorderedAccess& other)
{
	return m_Id == other.m_Id;
}

UnorderedAccess::~UnorderedAccess()
{
	delete m_pUploadResource;
	delete m_pDefaultResource;
	delete m_pUAV;
}

Resource* const UnorderedAccess::GetUploadResource() const
{
	return m_pUploadResource;
}

const Resource* const UnorderedAccess::GetDefaultResource() const
{
	return m_pDefaultResource;
}

const UnorderedAccessView* const UnorderedAccess::GetUAV() const
{
	return m_pUAV;
}
