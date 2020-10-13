#ifndef TEXTURE2DGUI_H
#define TEXTURE2DGUI_H

#include "Texture.h"

class Texture2DGUI : public Texture
{
public:
	Texture2DGUI(const std::wstring& filePath);
	virtual ~Texture2DGUI();

	bool Init(ID3D12Device5* device, DescriptorHeap* descriptorHeap);

private:
};

#endif