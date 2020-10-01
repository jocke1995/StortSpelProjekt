#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Core.h"
#include "Texture.h"
class Resource;
class CommandInterface;
class ShaderResourceView;
class DescriptorHeap;

// This class loads texture using DDS-loader
// It will be used for textures with mipmaps.

// If all you want is a 2D texture without mipmaps (example usage is for GUIs, then use 'Texture2DGUI' instead)
class Texture2D : public Texture
{
public:
	Texture2D();
	virtual ~Texture2D();

	bool Init(const std::wstring& filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap);

private:
};

#endif
