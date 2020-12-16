#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H

#include "Model.h"
#include "Animation.h"

// Forward declarations
struct Animation;
struct SkeletonNode;
struct VertexWeight;
struct TransformKey;

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

    // Uses dynamic allocation. Don't forget to delete the clone.
    SkeletonNode* CloneSkeleton();
    std::vector<Animation*> GetAnimations();

private:
    SkeletonNode* m_pSkeleton;
    std::vector<Animation*> m_Animations;

    unsigned int m_Id = 0;
};

#endif
