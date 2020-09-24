#ifndef MODEL_H
#define MODEL_H

#include "Core.h"


class Mesh;
class Material;
class Resource;
class ShaderResourceView;
class DescriptorHeap;
struct SlotInfo;
struct Animation;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

class Model
{
public:
    Model(const std::wstring* path,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
        std::vector<Material*>* materials);
    virtual ~Model();

    const std::wstring* GetPath() const;
    unsigned int GetSize() const;

    // Mesh
    Mesh* GetMeshAt(unsigned int index) const;

    // Material
    Material* GetMaterialAt(unsigned int index) const;

    // SlotInfo
    const SlotInfo* GetSlotInfoAt(unsigned int index) const;

private:
    std::wstring m_Path;
    unsigned int m_Size = 0;
    std::vector<Mesh*> m_Meshes;
    std::vector<Animation*> m_Animations;
    std::vector<Material*> m_Materials;
    std::vector<SlotInfo> m_SlotInfos;
};

#endif
