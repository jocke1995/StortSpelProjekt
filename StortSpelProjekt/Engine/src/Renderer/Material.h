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
    Material(const std::string* path, std::map<TEXTURE_TYPE, Texture*>* textures);
    virtual ~Material();

    std::string GetPath() const;

    // Material
    Texture* GetTexture(unsigned int index) const;

private:
    std::string m_Path;
    unsigned int m_Size = 0;
    std::map<TEXTURE_TYPE, Texture*> m_Textures;
};

#endif
