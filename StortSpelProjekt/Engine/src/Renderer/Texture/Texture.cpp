#include "stdafx.h"
#include "Texture.h"

#include "../Resource.h"
#include "../CommandInterface.h"
#include "../ShaderResourceView.h"
#include "../DescriptorHeap.h"

#include "TextureFunctions.h"

Texture::Texture()
{

}

Texture::~Texture()
{
	// TODO: Cleanup deconstructor
	if (m_pImageData != nullptr)
	{
		delete m_pImageData;
	}

	if (m_pDefaultResource != nullptr)
	{
		delete m_pDefaultResource;
	}

	if (m_pUploadResource != nullptr)
	{
		delete m_pUploadResource;
	}

	if (m_pSRV != nullptr)
	{
		delete m_pSRV;
	}
	
	free(const_cast<void*>(m_SubresourceData.pData));
}

const UINT Texture::GetDescriptorHeapIndex() const
{
	return m_pSRV->GetDescriptorHeapIndex();
}

bool Texture::Init(std::wstring filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap)
{
	m_FilePath = filePath;
	unsigned int descriptorHeapIndex = descriptorHeap->GetNextDescriptorHeapIndex(0);

	int imageBytesPerRow;

	// Load image Data
	unsigned int byteSize = LoadImageDataFromFile(&m_pImageData, &m_ResourceDescription, filePath, &imageBytesPerRow);
	if (byteSize == 0)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create texture: \'%s\'.\n", to_string(filePath).c_str());
		return false;
	}

	// Default heap
	m_pDefaultResource = new Resource(
		device,
		&m_ResourceDescription,
		nullptr,
		m_FilePath + L"_DEFAULT_RESOURCE",
		D3D12_RESOURCE_STATE_COMMON);

	// Footprint
	UINT64 textureUploadBufferSize;
	device->GetCopyableFootprints(
		&m_ResourceDescription,
		0, 1, 0,
		nullptr, nullptr, nullptr,
		&textureUploadBufferSize);

	// Upload heap
	m_pUploadResource = new Resource(device,
		textureUploadBufferSize,
		RESOURCE_TYPE::UPLOAD,
		m_FilePath + L"_UPLOAD_RESOURCE");

	// Create srv
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = m_ResourceDescription.Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	m_pSRV = new ShaderResourceView(
		device,
		descriptorHeap,
		&desc,
		m_pDefaultResource);

	// Set SubResource info
	m_SubresourceData.pData = &m_pImageData[0]; // pointer to our image data
	m_SubresourceData.RowPitch = imageBytesPerRow;
	m_SubresourceData.SlicePitch = imageBytesPerRow * m_ResourceDescription.Height;

	return true;
}