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

    void InitConstantBuffer(ID3D12Device5* device5, DescriptorHeap* CBV_UAV_SRV_heap);

    const ConstantBuffer* GetConstantBuffer() const;
    const std::vector<DirectX::XMFLOAT4X4>* GetUploadMatrices() const;

    bool PlayAnimation(std::string animationName, bool loop);
    void Update(double dt);
    void TempPlayAnimation();
    void TempPauseAnimation();
    void ResetAnimations();

private:
    // Will run in PlayAnimation().
    bool endAnimation();
    // Should run while there is an ongoing animation transition.
    void blendAnimations(double dt);
    // Will bind the blendAnimationState to the skeleton.
    void bindBlendedAnimation(SkeletonNode* node);

    // Will bind the animation to the skeleton.
    void bindAnimation(SkeletonNode* node, Animation* animation);
    // Calculates the finished transformations for the animation bound to the skeleton.
    void updateSkeleton(SkeletonNode* node, DirectX::XMMATRIX parentTransform);
    SkeletonNode* findNode(SkeletonNode* root, std::string nodeName);


    bool m_AnimationIsPaused = false;
    double m_BlendTransitionTime = ANIMATION_TRANSITION_TIME;
    double m_BlendTimeElapsed = 0;
    std::map<std::string, TransformKey> m_BlendAnimationState;
    Animation* m_pReactivateAnimation;
    Animation* m_pQueuedAnimation;
    std::vector<Animation*> m_pPendingAnimations;
    std::vector<Animation*> m_pActiveAnimations;
    std::vector<Animation*> m_pEndingAnimations;
    SkeletonNode* m_pSkeleton;
    std::vector<Animation*> m_Animations;
    DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
    std::vector<DirectX::XMFLOAT4X4> m_UploadMatrices;

    ConstantBuffer* m_pCB = nullptr;

    unsigned int m_Id = 0;
};

#endif
