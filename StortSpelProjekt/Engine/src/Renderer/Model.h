#ifndef MODEL_H
#define MODEL_H

#include "Core.h"


class Mesh;
class Material;
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
    Model(const std::wstring* path,
        std::vector<Mesh*>* meshes,
        std::vector<Material*>* materials);
    virtual ~Model();

    const std::wstring& GetPath() const;
    unsigned int GetSize() const;

    // Mesh
    Mesh* GetMeshAt(unsigned int index) const;
    void SetMeshAt(unsigned int index, Mesh* mesh);

    // Material
    Material* GetMaterialAt(unsigned int index) const;
    void SetMaterialAt(unsigned int index, Material* material);

    // SlotInfo
    const SlotInfo* GetSlotInfoAt(unsigned int index) const;

    double3 GetModelDim();

protected:
    friend class Renderer;
    friend class AssetLoader;

    void updateSlotInfo();

    std::wstring m_Path;
    unsigned int m_Size = 0;

    std::vector<Mesh*> m_Meshes;
    std::vector<Material*> m_Materials;
    std::vector<SlotInfo> m_SlotInfos; 

    double3 m_ModelDim;
};

#endif
