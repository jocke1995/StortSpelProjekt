#ifndef MATERIAL_H
#define MATERIAL_H

#include "Core.h"

class Texture;
class Resource;
class ShaderResourceView;
class DescriptorHeap;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

class Material
{
public:
    Material(const std::wstring* path, std::map<TEXTURE_TYPE, Texture*>* textures);
    virtual ~Material();

    const std::wstring* GetPath() const;

    // Material
    Texture* GetTexture(unsigned int index) const;

private:
    std::wstring m_Name;
    std::map<TEXTURE_TYPE, Texture*> m_Textures;
};

#endif
