#include "stdafx.h"
#include "Texture2D.h"

#include "../GPUMemory/Resource.h"
#include "../CommandInterface.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../DescriptorHeap.h"

// For loading textures
#include "TextureFunctions.h"

Texture2D::Texture2D(const std::wstring& filePath)
	: Texture(filePath)
{
	m_Type = TEXTURE_TYPE::TEXTURE2D;
	m_SubresourceData.resize(1); // 1 subresource
}

Texture2D::~Texture2D()
{
	
}

bool Texture2D::Init(ID3D12Device5* device, DescriptorHeap* descriptorHeap)
{
	HRESULT hr;

	m_pDefaultResource = new Resource();
	// DDSLoader uses this data type to load the image data
	// converts this to m_pImageData when it is used.
	std::unique_ptr<uint8_t[]> m_DdsData;
	
	// Loads the texture and creates a default resource;
	Cryptor::DecryptBinary(Cryptor::GetGlobalKey(), to_string(m_FilePath).c_str(), "DecryptedTexture.dds");
	hr = DirectX::LoadDDSTextureFromFile(device, L"DecryptedTexture.dds", reinterpret_cast<ID3D12Resource**>(m_pDefaultResource->GetID3D12Resource1PP()), m_DdsData, m_SubresourceData);
	remove("DecryptedTexture.dds");

	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create texture: \'%s\'.\n", to_string(m_FilePath).c_str());
		delete m_pDefaultResource;
		m_pDefaultResource = nullptr;
		return false;
	}
	// Set resource desc created in LoadDDSTextureFromFile
	m_ResourceDescription = m_pDefaultResource->GetID3D12Resource1()->GetDesc();
	m_ImageBytesPerRow = m_SubresourceData[0].RowPitch;
	// copy m_DdsData to our BYTE* format

	m_pImageData = static_cast<BYTE*>(m_DdsData.get());
	m_DdsData.release(); // lose the pointer, let m_pImageData delete the data.

	// Footprint
	UINT64 textureUploadBufferSize;

	device->GetCopyableFootprints(
		&m_ResourceDescription,
		0, m_ResourceDescription.MipLevels, 0,
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
	desc.Texture2D.MipLevels = m_pDefaultResource->GetID3D12Resource1()->GetDesc().MipLevels;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.ResourceMinLODClamp = 0.0f;

	m_pSRV = new ShaderResourceView(
		device,
		descriptorHeap,
		&desc,
		m_pDefaultResource);

	CoInitialize(NULL);

	return true;
}
