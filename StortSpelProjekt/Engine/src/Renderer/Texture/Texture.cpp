#include "stdafx.h"
#include "Texture.h"

#include "../GPUMemory/Resource.h"
#include "../CommandInterface.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../DescriptorHeap.h"

Texture::Texture(const std::wstring& filePath)
{
	m_FilePath = filePath;
}

Texture::~Texture()
{
	if (m_pImageData != nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Texture::~Texture(): %s was not nullptr\n", "m_pImageData");
		delete m_pImageData;
	}

	if (m_pDefaultResource != nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Texture::~Texture(): %s was not nullptr\n", "m_pDefaultResource");
		delete m_pDefaultResource;
	}

	if (m_pUploadResource != nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Texture::~Texture(): %s was not nullptr\n", "m_pUploadResource");
		delete m_pUploadResource;
	}

	if (m_pSRV != nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Texture::~Texture(): %s was not nullptr\n", "m_pSRV");
		delete m_pSRV;
	}
}

const std::wstring& Texture::GetPath() const
{
	return m_FilePath;
}

TEXTURE_TYPE Texture::GetType() const
{
	return m_Type;
}

const UINT Texture::GetDescriptorHeapIndex() const
{
	return m_pSRV->GetDescriptorHeapIndex();
}