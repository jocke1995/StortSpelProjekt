#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include "Component.h"
#include "../Renderer/Animation.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"
class Entity;
class ConstantBuffer;
class AnimatedModel;
struct MovementInput;
struct MouseClick;

namespace component
{
    class AnimationComponent : public Component
    {
    public:
        AnimationComponent(Entity* parent);
        virtual ~AnimationComponent();

        void RenderUpdate(double dt);
        void OnInitScene();
        void OnUnInitScene();
        // Will reset the component. No need to re-initialize.
        void Reset();

        // Requires that the entity has a model.
        void Initialize();

        // Plays an animation
        bool PlayAnimation(std::string animationName, bool loop);

    private:
        friend class Renderer;
        friend class AnimatedDepthRenderTask;

        AnimatedModel* m_pAnimatedModel = nullptr;
        SkeletonNode* m_pSkeleton = nullptr;

        DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
        std::map<std::string, TransformKey> m_AnimationState;

        std::vector<Animation*> m_Animations;
        std::pair<Animation*, AnimationInfo> m_pPendingAnimation;
        std::pair<Animation*, AnimationInfo> m_pActiveAnimation;
        std::pair<Animation*, AnimationInfo> m_pEndingAnimation;
        std::pair<Animation*, AnimationInfo> m_pReactivateAnimation;
        std::pair<Animation*, AnimationInfo> m_pQueuedAnimation;

        double m_BlendTimeElapsed = 0;

        std::vector<DirectX::XMFLOAT4X4> m_UploadMatrices;
        ConstantBuffer* m_pCB = nullptr;

        // Will bind the AnimationState to the skeleton.
        void bindAnimation(SkeletonNode* node);
        // Calculates the finished transformations for the animation bound to the skeleton.
        void updateSkeleton(SkeletonNode* node, DirectX::XMMATRIX parentTransform);

        void createCBMatrices(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV);
        void deleteCBMatrices();
    };
}
#endif
