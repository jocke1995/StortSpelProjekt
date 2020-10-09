#include "stdafx.h"
#include "Texture2DGUI.h"

#include "../GPUMemory/Resource.h"
#include "../CommandInterface.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../DescriptorHeap.h"

// For loading textures
#include "TextureFunctions.h"

Texture2DGUI::Texture2DGUI()
	: Texture()
{
	m_Type = TEXTURE_TYPE::TEXTURE2DGUI;
}

Texture2DGUI::~Texture2DGUI()
{
}

bool Texture2DGUI::Init(const std::wstring& filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap)
{
	m_FilePath = filePath;
	
	// Load image Data (using WIC)
	unsigned int byteSize = LoadImageDataFromFile(&m_pImageData, &m_ResourceDescription, filePath, &m_ImageBytesPerRow);
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
	m_SubresourceData.push_back({});
	m_SubresourceData[0].pData = &m_pImageData[0]; // pointer to our image data
	m_SubresourceData[0].RowPitch = m_ImageBytesPerRow;
	m_SubresourceData[0].SlicePitch = m_ImageBytesPerRow * m_ResourceDescription.Height;
	
	return true;
}
