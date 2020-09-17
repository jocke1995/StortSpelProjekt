#include "stdafx.h"
#include "Texture.h"

#include "../Resource.h"
#include "../CommandInterface.h"
#include "../ShaderResourceView.h"
#include "../DescriptorHeap.h"

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
}

const UINT Texture::GetDescriptorHeapIndex() const
{
	return m_pSRV->GetDescriptorHeapIndex();
}