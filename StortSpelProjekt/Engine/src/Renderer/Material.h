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
    Material(const std::wstring* path, std::map<TEXTURE2D_TYPE, Texture*>* textures);
    virtual ~Material();

    const std::wstring* GetPath() const;
    Texture* GetTexture(TEXTURE2D_TYPE type) const;

    void SetTexture(TEXTURE2D_TYPE type, Texture* texture);

private:
    std::wstring m_Name;
    std::map<TEXTURE2D_TYPE, Texture*> m_Textures;
};

#endif
