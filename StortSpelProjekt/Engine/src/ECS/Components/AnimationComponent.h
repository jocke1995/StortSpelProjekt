#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include "Component.h"
class Entity;
class AnimatedModel;
struct MovementInput;

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

        void Initialize();

        void SetActiveAnimation(std::string animationName);

    private:
        void walkAnimation(MovementInput* evnt);
        AnimatedModel* m_pModel;
    };
}
#endif
