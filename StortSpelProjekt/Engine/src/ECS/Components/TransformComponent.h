#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "../../Renderer/Transform.h"
#include "Component.h"

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
        Transform* transform = nullptr;
    };
}

#endif

