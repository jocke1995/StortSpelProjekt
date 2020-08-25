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
	void UploadToDefault(ID3D12Device5* device, CommandInterface* commandInterface, ID3D12CommandQueue* cmdQueue);

	const UINT GetDescriptorHeapIndex() const;
	Resource* GetResource() const;

private:
	std::wstring filePath = L"";
	unsigned int bytesPerRow = 0;
	unsigned int imageSize = 0;
	BYTE* imageData = nullptr;

	ShaderResourceView* SRV = nullptr;

	D3D12_RESOURCE_DESC resourceDescription = {};
	Resource* resourceDefaultHeap = nullptr;
	Resource* resourceUploadHeap = nullptr;
	bool CreateTexture(std::wstring filePath, ID3D12Device5* device, UINT descriptorHeapIndex_SRV);

	// Temp until scene is properly loaded when a change of scene happens
	bool hasBeenUploadedToDefault = false;
};

#endif
