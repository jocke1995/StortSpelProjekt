#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "Model.h"

// Forward declarations
struct Animation;
struct SkeletonNode;
struct VertexWeight;


class AnimatedModel : public Model
{
public:
    AnimatedModel(const std::wstring* path,
        SkeletonNode* rootNode,
        std::map<unsigned int, VertexWeight>* perVertexBoneData,
        std::vector<Mesh*>* meshes,
        std::vector<Animation*>* animations,
        std::vector<Material*>* materials);
    virtual ~AnimatedModel();

    void Update(double dt);

private:
    void updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform);

    Animation* m_pActiveAnimation;
    std::vector<Animation*> m_Animations;
    SkeletonNode* m_pSkeleton;
    DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
    std::map<unsigned int, VertexWeight> m_PerVertexBoneData; // AKA weights
};









#endif
