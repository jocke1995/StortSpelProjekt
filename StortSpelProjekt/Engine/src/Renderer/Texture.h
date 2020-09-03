#ifndef TEXTURE_H
#define TEXTURE_H

#include "Core.h"
class Resource;
class CommandInterface;
class ShaderResourceView;
class DescriptorHeap;

class Texture
{
public:
	Texture();
	virtual ~Texture();
	
	bool Init(std::wstring filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap);

	const UINT GetDescriptorHeapIndex() const;

private:
	// CopyOnDemandTask & Renderer uses the private members of the texture class to upload data to the gpu
	friend class CopyOnDemandTask;
	friend class Renderer;

	std::wstring m_FilePath = L"";
	ShaderResourceView* m_pSRV = nullptr;
	D3D12_SUBRESOURCE_DATA m_SubresourceData = {};
	D3D12_RESOURCE_DESC m_ResourceDescription = {};
	Resource* m_pResourceDefaultHeap = nullptr;
	Resource* m_pResourceUploadHeap = nullptr;

	bool createTexture(std::wstring filePath, ID3D12Device5* device, UINT descriptorHeapIndex_SRV);



	// Temp until scene is properly loaded when a change of scene happens
	bool m_HasBeenUploadedToDefault = false;
};

#endif
