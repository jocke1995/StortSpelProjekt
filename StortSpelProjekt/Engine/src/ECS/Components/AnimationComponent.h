#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include "Component.h"
#include "../Renderer/Animation.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"
class Entity;
class ConstantBuffer;
class ShaderResourceView;
class UnorderedAccessView;
class Resource;
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

        // Plays an animation
        bool PlayAnimation(std::string animationName, bool loop);

    private:
        friend class Renderer;
        friend class AnimatedDepthRenderTask;
        friend class ForwardRenderTask;
        friend class ShadowRenderTask;
        
        AnimatedModel* m_pAnimatedModel = nullptr;
        SkeletonNode* m_pSkeleton = nullptr;

        DirectX::XMFLOAT4X4 m_GlobalInverseTransform;
        std::map<std::string, TransformKey> m_AnimationState;

        std::vector<Animation*> m_Animations;
        std::pair<Animation*, AnimationInfo> m_PendingAnimation;
        std::pair<Animation*, AnimationInfo> m_ActiveAnimation;
        std::pair<Animation*, AnimationInfo> m_EndingAnimation;
        std::pair<Animation*, AnimationInfo> m_ReactivateAnimation;
        std::pair<Animation*, AnimationInfo> m_QueuedAnimation;

        double m_BlendTimeElapsed = 0;

        std::vector<DirectX::XMFLOAT4X4> m_UploadMatrices;
        ConstantBuffer* m_pCB = nullptr;

        // Hack to kringå systemet
        std::vector<Resource*> m_DefaultResourceVertices;
        std::vector<ShaderResourceView*> m_SRVs;
        std::vector<UnorderedAccessView*> m_UAVs;

        // Requires that the entity has a model.
        void initialize(ID3D12Device5* device5, DescriptorHeap* dh_CBV_UAV_SRV);

        // Will bind the AnimationState to the skeleton.
        void bindAnimation(SkeletonNode* node);
        // Calculates the finished transformations for the animation bound to the skeleton.
        void updateSkeleton(SkeletonNode* node, DirectX::XMMATRIX parentTransform);

        void createCBMatrices(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV);
        void deleteCBMatrices();
    };
}
#endif
