#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "Component.h"
class Transform;

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
    };
}

#endif

