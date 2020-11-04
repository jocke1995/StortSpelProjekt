#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H

#include "Model.h"

// Forward declarations
struct Animation;
struct SkeletonNode;
struct VertexWeight;

class ConstantBuffer;
class DescriptorHeap;

struct ID3D12Device5;

static unsigned int s_AnimatedModelIdCounter = 0;

class AnimatedModel : public Model
{
public:
    AnimatedModel(const std::wstring* path,
        SkeletonNode* rootNode,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
        std::vector<Material*>* materials,
        unsigned int numBones);
    virtual ~AnimatedModel();

    void InitConstantBuffer(ID3D12Device5* device5, DescriptorHeap* CBV_UAV_SRV_heap);

    const ConstantBuffer* GetConstantBuffer() const;
    const std::vector<DirectX::XMFLOAT4X4>* GetUploadMatrices() const;

    void Update(double dt);

private:
    void updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform);

    Animation* m_pActiveAnimation;
    SkeletonNode* m_pSkeleton;
    std::vector<Animation*> m_Animations;
    DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
    std::vector<DirectX::XMFLOAT4X4> m_UploadMatrices;

    ConstantBuffer* m_pCB = nullptr;

    unsigned int m_Id = 0;
};

#endif
