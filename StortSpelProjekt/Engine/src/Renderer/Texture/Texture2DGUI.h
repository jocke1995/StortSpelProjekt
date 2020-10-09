#ifndef TEXTURE2DGUI_H
#define TEXTURE2DGUI_H

#include "Texture.h"

class Texture2DGUI : public Texture
{
public:
	Texture2DGUI();
	virtual ~Texture2DGUI();

	bool Init(const std::wstring& filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap);

private:
};

#endif