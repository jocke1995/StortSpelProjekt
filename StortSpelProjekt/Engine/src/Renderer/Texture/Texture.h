#ifndef TEXTURE_H
#define TEXTURE_H

#include "Core.h"
#include "../../Headers/d3dx12.h"

class Resource;
class CommandInterface;
class ShaderResourceView;
class DescriptorHeap;

class Texture
{
public:
	Texture();
	virtual ~Texture();

	virtual bool Init(const std::wstring& filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap) = 0;

	TEXTURE_TYPE GetType() const;
	const unsigned int GetDescriptorHeapIndex() const;

protected:
	// CopyOnDemandTask & Renderer uses the private members of the texture class to upload data to the gpu
	friend class CopyOnDemandTask;
	friend class Renderer;
	friend class Text;

	TEXTURE_TYPE m_Type = TEXTURE_TYPE::UNKNOWN;

	BYTE* m_pImageData = nullptr;
	int m_ImageBytesPerRow = 0;
	std::wstring m_FilePath = L"";
	ShaderResourceView* m_pSRV = nullptr;
	std::vector<D3D12_SUBRESOURCE_DATA> m_SubresourceData;
	D3D12_RESOURCE_DESC m_ResourceDescription = {};
	Resource* m_pDefaultResource = nullptr;
	Resource* m_pUploadResource = nullptr;

	// Temp until scene is properly loaded when a change of scene happens
	bool m_HasBeenUploadedToDefault = false;
};

#endif
