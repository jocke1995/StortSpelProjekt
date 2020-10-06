#ifndef MODEL_H
#define MODEL_H

#include "Core.h"


class Mesh;
class Texture;
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
    Model(const std::wstring path,
        SkeletonNode* rootNode,
        std::map<unsigned int, VertexWeight>* perVertexBoneData,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
        std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures);
    virtual ~Model();

    void Update();

    std::wstring GetPath() const;
    unsigned int GetSize() const;

    // Mesh
    Mesh* GetMeshAt(unsigned int index);

    // Material
    std::map<TEXTURE_TYPE, Texture*>* GetTexturesAt(unsigned int index);

    // SlotInfo
    SlotInfo* GetSlotInfoAt(unsigned int index);

private:

    void updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform);

    std::wstring m_Path;
    unsigned int m_Size = 0;

    std::vector<Mesh*> m_Meshes;
    std::vector<std::map<TEXTURE_TYPE, Texture*>> m_Textures;
    std::vector<SlotInfo> m_SlotInfos;

    Animation* m_pActiveAnimation;
    std::vector<Animation*> m_Animations;
    SkeletonNode* m_pSkeleton;
    DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
    std::map<unsigned int, VertexWeight> m_PerVertexBoneData; // AKA weights
};

#endif
