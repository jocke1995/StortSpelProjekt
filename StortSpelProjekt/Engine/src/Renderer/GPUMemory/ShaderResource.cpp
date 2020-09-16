#include "stdafx.h"
#include "ShaderResource.h"

#include "Resource.h"
#include "ShaderResourceView.h"
#include "../DescriptorHeap.h"

ShaderResource::ShaderResource(
    ID3D12Device5* device,
    unsigned int entrySize,
    std::wstring resourceName,
    D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
    DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
    m_pUploadResource = new Resource(device, entrySize, RESOURCE_TYPE::UPLOAD, resourceName + L"_UPLOAD");
    m_pDefaultResource = new Resource(device, entrySize, RESOURCE_TYPE::DEFAULT, resourceName + L"_DEFAULT");
    m_pSRV = new ShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, srvDesc, m_pDefaultResource);

    m_pId = s_SrCounter++;
}

bool ShaderResource::operator==(const ShaderResource& other)
{
    return m_pId == other.m_pId;
}

ShaderResource::~ShaderResource()
{
    delete m_pUploadResource;
    delete m_pDefaultResource;
    delete m_pSRV;
}

Resource* const ShaderResource::GetUploadResource() const
{
    return m_pUploadResource;
}

const Resource* const ShaderResource::GetDefaultResource() const
{
    return m_pDefaultResource;
}

const ShaderResourceView* const ShaderResource::GetSRV() const
{
    return m_pSRV;
}
