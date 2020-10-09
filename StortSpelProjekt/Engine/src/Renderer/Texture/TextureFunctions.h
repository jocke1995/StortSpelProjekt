#ifndef TEXTUREFUNCTIONS_H
#define TEXTUREFUNCTIONS_H

// For loading DDS (cubemap and mipmapped textures)
#include "DDSTextureLoader12.h"

// For loading common fileformats without mipmaps (WIC-loader)
unsigned int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC* resourceDesc, std::wstring path, int* bytesPerRow);

#endif