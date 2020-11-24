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

    bool AddActiveAnimation(std::string animationName, bool loop);
    bool EndActiveAnimation(std::string animationName);
    void Update(double dt);
    void PlayAnimation();
    void PauseAnimation();
    void ResetAnimation();

private:
    // Should run for a while after a activeAnimation is added.
    void blendAnimations(double dt);
    void bindBlendedAnimation(SkeletonNode* node);

    void bindAnimation(SkeletonNode* node, Animation* animation);   // Attach the currentStateTransforms of the animation to the skeleton.
    void updateSkeleton(SkeletonNode* node, DirectX::XMMATRIX parentTransform);
    SkeletonNode* findNode(SkeletonNode* root, std::string nodeName);


    bool m_AnimationIsPaused = false;
    double blendTransitionTime = ANIMATION_TRANSITION_TIME;
    double blendTimeElapsed = 0;
    std::map<std::string, TransformKey> m_BlendAnimationState;
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
