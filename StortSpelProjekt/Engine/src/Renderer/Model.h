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
struct NodeTemp;
struct NodeAnimation;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

class Model
{
public:
    Model(const std::wstring path,
        NodeTemp* rootNode,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
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
    void updateAnimations();
    void updateBones(float animationTime, NodeTemp* node, DirectX::XMMATRIX parentTransform);
    DirectX::XMMATRIX interpolateScaling(float animationTime, NodeAnimation* nodeAnimation);
    DirectX::XMMATRIX interpolateRotation(float animationTime, NodeAnimation* nodeAnimation);
    DirectX::XMMATRIX interpolateTranslation(float animationTime, NodeAnimation* nodeAnimation);

    std::wstring m_Path;
    unsigned int m_Size = 0;
    Animation* m_pActiveAnimation;
    NodeTemp* m_pRootNode;
    std::vector<Mesh*> m_Meshes;
    std::vector<Animation*> m_Animations;
    std::vector<std::map<TEXTURE_TYPE, Texture*>> m_Textures;
    std::vector<SlotInfo> m_SlotInfos;
};

#endif
