#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include "Core.h"
#include "Texture.h"
class Resource;
class CommandInterface;
class ShaderResourceView;
class DescriptorHeap;

class TextureCubeMap : public Texture
{
public:
	TextureCubeMap(const std::wstring& filePath);
	virtual ~TextureCubeMap();

	bool Init(ID3D12Device5* device, DescriptorHeap* descriptorHeap);

private:
	friend class CopyOnDemandTask;
};

#endif
