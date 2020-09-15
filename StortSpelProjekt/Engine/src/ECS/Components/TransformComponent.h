#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "Component.h"
class Transform;
class MovementInput;
class MouseMovement;
class ModifierInput;

namespace component
{
    class TransformComponent : public Component
    {
    public:
        TransformComponent(Entity* parent);
        virtual ~TransformComponent();

        void Update(double dt);

        Transform* GetTransform() const;
    private:
        Transform* m_pTransform = nullptr;
        bool m_CameraLocked;

        void setMovement(MovementInput* evnt);
        void setRotation(MouseMovement* evnt);
        void toggleCameraLock(ModifierInput* evnt);
    };
}

#endif

