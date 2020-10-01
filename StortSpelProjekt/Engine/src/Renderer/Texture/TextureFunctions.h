#ifndef TEXTUREFUNCTIONS_H
#define TEXTUREFUNCTIONS_H

// For loading DDS (cubemap)
#include "DDSTextureLoader12.h"

unsigned int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC* resourceDesc, std::wstring path, int* bytesPerRow);

#endif