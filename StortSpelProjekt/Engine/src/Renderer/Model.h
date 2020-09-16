#ifndef MODEL_H
#define MODEL_H

#include "Core.h"


class Mesh;
class Texture;
class Resource;
class ShaderResourceView;
class DescriptorHeap;
struct SlotInfo;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

class Model
{
public:
    Model(const std::wstring path,
        std::vector<Mesh*>* meshes,
        std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures);
    virtual ~Model();

    std::wstring GetPath() const;
    unsigned int GetSize() const;

    // Mesh
    Mesh* GetMeshAt(unsigned int index);

    // Material
    std::map<TEXTURE_TYPE, Texture*>* GetTexturesAt(unsigned int index);

    // SlotInfo
    SlotInfo* GetSlotInfoAt(unsigned int index);

private:
    std::wstring m_Path;
    unsigned int m_Size = 0;
    std::vector<Mesh*> m_Meshes;
    std::vector<std::map<TEXTURE_TYPE, Texture*>> m_Textures;
    std::vector<SlotInfo>  m_SlotInfos;
};

#endif
