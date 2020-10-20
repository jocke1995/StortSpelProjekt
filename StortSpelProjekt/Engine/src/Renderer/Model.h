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
struct SkeletonNode;
struct NodeAnimation;
struct VertexWeight;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

class Model
{
public:
    Model(const std::wstring* path,
        SkeletonNode* rootNode,
        std::map<unsigned int, VertexWeight>* perVertexBoneData,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
        std::vector<Material*>* materials);
    virtual ~Model();

    void Update(double dt);

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

    void updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform);
    void updateSlotInfo();

    std::wstring m_Path;
    unsigned int m_Size = 0;

    std::vector<Mesh*> m_Meshes;
    std::vector<Material*> m_Materials;
    std::vector<SlotInfo> m_SlotInfos; 

    Animation* m_pActiveAnimation;
    std::vector<Animation*> m_Animations;
    SkeletonNode* m_pSkeleton;
    DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
    std::map<unsigned int, VertexWeight> m_PerVertexBoneData; // AKA weights

    double3 m_ModelDim;
};

#endif
