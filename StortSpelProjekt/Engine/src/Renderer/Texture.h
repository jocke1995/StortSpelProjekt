#ifndef TEXTURE_H
#define TEXTURE_H

#include <wincodec.h>
#include "Resource.h"
#include "CommandInterface.h"
#include "ShaderResourceView.h"

enum TEXTURE_TYPE
{
	AMBIENT,
	DIFFUSE,
	SPECULAR,
	NORMAL,
	EMISSIVE,
	NUM_TEXTURE_TYPES
};

class Texture
{
public:
	Texture();
	~Texture();
	
	bool Init(std::wstring filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap);

	const UINT GetDescriptorHeapIndex() const;

private:
	// CopyOnDemandTask uses the private members of the texture class to upload data to the gpu
	friend class CopyOnDemandTask;
	D3D12_SUBRESOURCE_DATA m_SubresourceData = {};
	std::wstring filePath = L"";

	ShaderResourceView* SRV = nullptr;

	D3D12_RESOURCE_DESC resourceDescription = {};
	Resource* resourceDefaultHeap = nullptr;
	Resource* resourceUploadHeap = nullptr;
	bool CreateTexture(std::wstring filePath, ID3D12Device5* device, UINT descriptorHeapIndex_SRV);

	// Temp until scene is properly loaded when a change of scene happens
	bool hasBeenUploadedToDefault = false;
};

#endif
