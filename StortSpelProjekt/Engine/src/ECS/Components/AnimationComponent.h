#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include "Component.h"
class Entity;
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

        // Requires that the entity has a model.
        void Initialize();

    private:
        void walkAnimation(MovementInput* evnt);
        void attackAnimation(MouseClick* evnt);
        AnimatedModel* m_pAnimatedModel;
    };
}
#endif
